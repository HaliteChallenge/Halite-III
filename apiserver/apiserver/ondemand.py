"""
Internal API for running games on-demand (e.g. tutorials/web editor).
"""
import google.cloud.datastore as gcloud_datastore

from . import config, model


ONDEMAND_KIND = "ondemand"


def key_from_user_id(user_id):
    return gcloud_datastore.Key(
        ONDEMAND_KIND,
        str(user_id),
        project=config.GCLOUD_PROJECT,
    )


def check_status(user_id):
    client = model.get_datastore_client()
    query = client.query(kind=ONDEMAND_KIND)
    query.add_filter("status", "=", "pending")
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
    })
    client.put(entity)


def continue_game(user_id, num_turns):
    pass


def pending_task():
    client = model.get_datastore_client()
    query = client.query(kind=ONDEMAND_KIND)
    query.add_filter("status", "=", "pending")
    query.order = ["-last_updated"]

    result = list(query.fetch(limit=1))
    return result[0] if result else None
