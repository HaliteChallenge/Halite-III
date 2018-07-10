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


def launch(user_id, opponents, environment_parameters):
    client = model.get_datastore_client()
    entity = gcloud_datastore.Entity(key_from_user_id(user_id))
    entity.update({
        "status": "pending",
        "opponents": opponents,
        "environment_parameters": environment_parameters,
        "last_updated": datetime.datetime.now(datetime.timezone.utc),
        "retries": 0,
    })
    client.put(entity)


def continue_game(user_id, num_turns):
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
    elif task["status"] != "completed":
        raise util.APIError(
            400,
            message="Ondemand game not ready for user {}. Please wait.".format(user_id))

    task.update({
        "status": "pending",
        "last_updated": datetime.datetime.now(datetime.timezone.utc),
        "retries": 0,
    })
    task["environment_parameters"]["from-snapshot"] = task["game_output"]["final_snapshot"]
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
    client = model.get_datastore_client()
    query = client.query(kind=ONDEMAND_KIND)
    query.key_filter(key_from_user_id(user_id))

    result = list(query.fetch(limit=1))
    if not result:
        return

    task = result[0]
    task["status"] = "completed"
    task["game_output"] = game_output
    task["last_updated"] = datetime.datetime.now(datetime.timezone.utc)
    task["retries"] = 0

    # TODO: upload error logs (just store them in the blob if small enough?)
    bucket = model.get_ondemand_replay_bucket()
    replay_key = "ondemand_{}".format(user_id)
    blob = gcloud_storage.Blob(replay_key, bucket, chunk_size=262144)
    blob.upload_from_file(files["replay"])

    client.put(task)
