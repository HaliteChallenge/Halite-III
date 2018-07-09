"""
Coordinator API for running games on-demand (e.g. for the web editor
and tutorial).
"""

from .. import model, ondemand, util

from .blueprint import coordinator_api

@coordinator_api.route("/ondemand")
def ondemand_task():
    task = ondemand.pending_task()
    return util.response_success({
        "task": task,
    })


@coordinator_api.route("/ondemand_result", methods=["POST"])
def ondemand_result():
    pass
