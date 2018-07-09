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
        result = dict(task)
        result["type"] = "ondemand"
        result["challenge"] = None
        # TODO: fill in all proper parameters
        result["users"] = [
            {
                "user_id": task.key.id,
                "bot_id": 0,
            },
        ]
        return util.response_success(result)
    else:
        return util.response_success({
            "type": "notask",
        })


@coordinator_api.route("/ondemand_result", methods=["POST"])
def ondemand_result():
    pass
