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

        users = []
        for opponent in task["opponents"]:
            if opponent["bot_id"] == "self":
                users.append({
                    "user_id": task.key.id,
                    "bot_id": "editor",
                    "username": opponent["name"],
                    "requires_compilation": True,
                    "version_number": 1,
                })
            else:
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

    files = {
        "replay": flask.request.files[replay_name],
    }

    if "0" in game_output["error_logs"]:
        filepath = game_output["error_logs"]["0"]
        filename = os.path.basename(filepath)
        files["error_log"] = flask.request.files[filename]

    ondemand.update_task(task_user_id, game_output, files)

    return util.response_success()


@coordinator_api.route("/ondemand_compile", methods=["POST"])
def ondemand_compile():
    """Save the results of an ondemand game that failed compilation."""

    task_user_id = int(flask.request.values["task_user_id"])

    ondemand.update_task(task_user_id, None, {
        "compile_error": flask.request.values["log"],
    })

    return util.response_success()
