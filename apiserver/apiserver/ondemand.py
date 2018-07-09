"""
Internal API for running games on-demand (e.g. tutorials/web editor).
"""

from . import model


ONDEMAND_KIND = "ondemand"


def launch(user_id, opponents, num_turns):
    pass


def continue_game(user_id, num_turns):
    pass


def pending_task():
    client = model.get_datastore_client()
    query = client.query(kind=ONDEMAND_KIND)
    query.add_filter("status", "=", "pending")
    query.order = ["-last_updated"]

    return list(query.fetch(limit=1))
