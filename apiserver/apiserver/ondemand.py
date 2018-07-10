"""
Internal API for running games on-demand (e.g. tutorials/web editor).
"""
import datetime
import time

import google.cloud.datastore as gcloud_datastore

from . import config, model


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


def launch(user_id, opponents, num_turns):
    client = model.get_datastore_client()
    entity = gcloud_datastore.Entity(key_from_user_id(user_id))
    entity.update({
        "status": "pending",
        "opponents": opponents,
        "num_turns": num_turns,
        "width": 128,
        "height": 128,
        "last_updated": datetime.datetime.now(),
    })
    client.put(entity)


def continue_game(user_id, num_turns):
    pass


TASK_CONFLICT_BACKOFF = 1
TASK_CONFLICT_FACTOR = 2
TASK_CONFLICT_BACKOFF_MAX = 16


def pending_task():
    client = model.get_datastore_client()
    current_backoff = TASK_CONFLICT_BACKOFF

    while True:
        query = client.query(kind=ONDEMAND_KIND)
        query.add_filter("status", "=", "pending")
        query.order = ["-last_updated"]

        result = list(query.fetch(limit=1))

        if result:
            with client.transaction() as xact:
                task = client.get(result[0].key)
                if task["status"] != "pending":
                    # Wait and retry
                    time.sleep(current_backoff)
                    current_backoff = min(
                        TASK_CONFLICT_BACKOFF_MAX,
                        TASK_CONFLICT_FACTOR * current_backoff)
                    continue

                task["status"] = "running"
                xact.put(task)
                return task
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

    # TODO: upload replay and error logs

    client.put(task)
