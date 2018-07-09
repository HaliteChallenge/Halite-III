"""
Coordinator API for running games on-demand (e.g. for the web editor
and tutorial).
"""

from .. import model, ondemand, util

from .blueprint import coordinator_api

@coordinator_api.route("/ondemand")
def ondemand_task():
    task = ondemand.pending_task()
    if task:
        return util.response_success({
            "type": "ondemand",
            "task": task,
        })
    else:
        return util.response_success({
            "type": "notask",
        })


@coordinator_api.route("/ondemand_result", methods=["POST"])
def ondemand_result():
    pass
