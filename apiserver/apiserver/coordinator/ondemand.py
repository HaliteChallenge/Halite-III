"""
Coordinator API for running games on-demand (e.g. for the web editor
and tutorial).
"""
import flask
import json
import os

from .. import model, ondemand, util

from .blueprint import coordinator_api

@coordinator_api.route("/ondemand")
def ondemand_task():
    task = ondemand.pending_task()
    if task:
        result = dict(task)
        result["type"] = "ondemand"
        result["challenge"] = None
        result["task_user_id"] = task.key.id

        users = [
            {
                "user_id": task.key.id,
                "bot_id": "editor",
                "username": "my-editor-bot",
                "requires_compilation": True,
                "version_number": 1,
            },
        ]
        for opponent in task["opponents"]:
            users.append({
                "user_id": "gym",
                "bot_id": opponent["bot_id"],
                "username": opponent["name"],
                "version_number": 1,
            })

        result["users"] = users

        return util.response_success(result)
    else:
        return util.response_success({
            "type": "notask",
        })


@coordinator_api.route("/ondemand_result", methods=["POST"])
def ondemand_result():
    """Save the results of an ondemand game into document storage."""

    if ("game_output" not in flask.request.values or
            "users" not in flask.request.values):
        raise util.APIError(
            400, message="Please provide both the game output and users.")

    game_output = json.loads(flask.request.values["game_output"])
    replay_name = os.path.basename(game_output["replay"])
    task_user_id = int(flask.request.values["task_user_id"])
    if replay_name not in flask.request.files:
        raise util.APIError(
            400, message="Replay file not found in uploaded files.")

    print(game_output)

    ondemand.update_task(task_user_id, game_output, {
        "replay": flask.request.files[replay_name],
        # TODO: error logs
    })

    return util.response_success()
