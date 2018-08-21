"""
Internal API for running games on-demand (e.g. tutorials/web editor).
"""
import datetime
import time

import google.cloud.datastore as gcloud_datastore
import google.cloud.storage as gcloud_storage

from . import config, model, util


ONDEMAND_KIND = "ondemand"


def key_from_user_id(user_id):
    """
    Create a datastore key from a user ID.

    Each user has at most 1 ondemand game task.
    """
    return gcloud_datastore.Key(
        ONDEMAND_KIND,
        user_id,
        project=config.GCLOUD_PROJECT,
    )


def check_status(user_id):
    client = model.get_datastore_client()
    query = client.query(kind=ONDEMAND_KIND)
    query.key_filter(key_from_user_id(user_id))

    result = list(query.fetch(limit=1))
    return result[0] if result else None


def launch(user_id, opponents, environment_parameters, metadata):
    """
    Create a new ondemand game task.
    """
    client = model.get_datastore_client()
    entity = gcloud_datastore.Entity(key_from_user_id(user_id))
    entity.update({
        "status": "created",
        "opponents": opponents,
        "environment_parameters": environment_parameters,
        "last_updated": datetime.datetime.now(datetime.timezone.utc),
        "retries": 0,
        # Used to associate a game with a tutorial or something like
        # that
        "metadata": metadata,
        "objective": {
            "completed": False,
        },
    })
    client.put(entity)


def continue_game(user_id, num_turns, snapshot_index):
    """
    Request that an ondemand game be resumed.
    """
    client = model.get_datastore_client()
    query = client.query(kind=ONDEMAND_KIND)
    query.key_filter(key_from_user_id(user_id))

    result = list(query.fetch(limit=1))
    if not result:
        raise util.APIError(
            404,
            message="Ondemand game not found for user {}.".format(user_id))

    task = result[0]
    if task["status"] == "failed":
        raise util.APIError(
            400,
            message="Ondemand game failed for user {}. Please restart.".format(user_id))
    elif task["status"] not in ("completed", "created"):
        raise util.APIError(
            400,
            message="Ondemand game not ready for user {}. Please wait.".format(user_id))

    # Resume game from snapshot of state if this is not the first time
    # we're starting it
    if task["status"] == "completed" and "game_output" in task:
        task["environment_parameters"]["from-snapshot"] = \
            task["snapshots"][snapshot_index]["snapshot"]

    task.update({
        "status": "pending",
        "last_updated": datetime.datetime.now(datetime.timezone.utc),
        "retries": 0,
        "objective": {
            "completed": False,
        },
    })

    task["environment_parameters"]["turn-limit"] = num_turns

    client.put(task)


# Base seconds to wait if task assignment conflicts.
TASK_CONFLICT_BACKOFF = 1

# Multiplicative increase for wait time
TASK_CONFLICT_FACTOR = 2

# Maximum wait time
TASK_CONFLICT_BACKOFF_MAX = 16

# Maximum minutes before task is stale and can be rescheduled.
TASK_MAX_AGE = 5

# Maximum number of times a task will be retried.
TASK_MAX_RETRIES = 3


def pending_task():
    """
    Look for a pending ondemand game task.
    """
    client = model.get_datastore_client()
    current_backoff = TASK_CONFLICT_BACKOFF

    while True:
        # Search first for games that are stuck
        stale_cutoff = datetime.datetime.now(datetime.timezone.utc) - datetime.timedelta(minutes=TASK_MAX_AGE)
        query = client.query(kind=ONDEMAND_KIND)
        query.add_filter("status", "=", "running")
        query.add_filter("last_updated", "<", stale_cutoff)
        query.order = ["-last_updated"]

        result = list(query.fetch(limit=1))

        # Search for regular games
        if not result:
            query = client.query(kind=ONDEMAND_KIND)
            query.add_filter("status", "=", "pending")
            query.order = ["-last_updated"]

            result = list(query.fetch(limit=1))

        if result:
            # Make sure tasks are only assigned once.
            with client.transaction() as xact:
                task = client.get(result[0].key)
                if (task["status"] == "running" and
                    task["last_updated"] < stale_cutoff and
                    task["retries"] > TASK_MAX_RETRIES):
                    task["status"] = "failed"
                    task["last_updated"] = datetime.datetime.now(datetime.timezone.utc)
                    xact.put(task)

                elif task["status"] == "pending" or (
                        task["status"] == "running" and
                        task["last_updated"] < stale_cutoff):
                    task["status"] = "running"
                    task["last_updated"] = datetime.datetime.now(datetime.timezone.utc)
                    task["retries"] += 1
                    xact.put(task)
                    return task

                # Otherwise, wait and retry
                time.sleep(current_backoff)
                current_backoff = min(
                    TASK_CONFLICT_BACKOFF_MAX,
                    TASK_CONFLICT_FACTOR * current_backoff)
        else:
            # No task available
            return None


def update_task(user_id, game_output, files):
    """
    Update the status of an ondemand game after play.
    """
    client = model.get_datastore_client()
    query = client.query(kind=ONDEMAND_KIND)
    query.key_filter(key_from_user_id(user_id))

    result = list(query.fetch(limit=1))
    if not result:
        return

    task = result[0]
    task["status"] = "completed"

    current_time = datetime.datetime.now(datetime.timezone.utc)

    if game_output:
        # Track history of game state snapshots, so that user can
        # rewind. Use case: user takes next step in tutorial and fails;
        # needs previous game state, not current game state, to continue.
        if "snapshots" not in task:
            task["snapshots"] = []

        # task["snapshots"].append({
        #     "snapshot": game_output["final_snapshot"],
        #     "updated_at": current_time,
        # })

    del game_output["final_snapshot"]
    task["game_output"] = game_output
    task["last_updated"] = current_time
    task["retries"] = 0

    # TODO: once we track tutorials, have this reflect tutorial
    # objective completion status instead of victory

    # TODO: will the relevant player always be player 0?
    task["objective"] = {
        "completed": game_output["stats"]["0"]["rank"] == 1,
    }

    if "replay" in files:
        bucket = model.get_ondemand_replay_bucket()
        replay_key = "ondemand_{}".format(user_id)
        blob = gcloud_storage.Blob(replay_key, bucket, chunk_size=262144)
        blob.upload_from_file(files["replay"])

    if "error_log" in files:
        bucket = model.get_ondemand_replay_bucket()
        log_key = "ondemand_log_{}".format(user_id)
        blob = gcloud_storage.Blob(log_key, bucket, chunk_size=262144)
        blob.upload_from_file(files["error_log"])
        task["error_log"] = True
        task["crashed"] = game_output["terminated"].get("0", False)
    else:
        task["error_log"] = None
        task["crashed"] = False

    if "bot_log" in files:
        bucket = model.get_ondemand_replay_bucket()
        log_key = "ondemand_bot_log_{}".format(user_id)
        blob = gcloud_storage.Blob(log_key, bucket, chunk_size=262144)
        blob.upload_from_file(files["bot_log"])
        task["bot_log"] = True
    else:
        task["bot_log"] = None

    if "compile_error" in files:
        task["compile_error"] = files["compile_error"]
    else:
        task["compile_error"] = None

    client.put(task)
