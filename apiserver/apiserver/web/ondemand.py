"""
Coordinator API for running games on-demand (e.g. for the web editor
and tutorial).
"""
import io

import flask
import google.cloud.exceptions as gcloud_exceptions
import google.cloud.storage as gcloud_storage

from .. import model, ondemand, util

from . import util as web_util
from .blueprint import web_api

@web_api.route("/ondemand", methods=["GET"])
@util.cross_origin(methods=["GET", "POST", "PUT"])
@web_util.requires_login(accept_key=False)
def check_ondemand(*, user_id):
    return util.response_success(ondemand.check_status(user_id) or {
        "status": "none",
    })


@web_api.route("/ondemand", methods=["PUT"])
@util.cross_origin(methods=["GET", "POST", "PUT"])
@web_util.requires_login(accept_key=False)
def start_ondemand(*, user_id):
    # TODO: specify opponents, or specify something like "web-ide" vs
    # "tutorial-1"

    # TODO: check that user has a bot and that it successfully
    # compiled
    ondemand.launch(user_id, [], 1)
    return util.response_success()


@web_api.route("/ondemand", methods=["POST"])
@util.cross_origin(methods=["GET", "POST", "PUT"])
@web_util.requires_login(accept_key=False)
def continue_ondemand(*, user_id):
    # TODO: specify turns
    ondemand.continue_game(user_id, 1)
    return util.response_success()


@web_api.route("/ondemand/replay", methods=["GET"])
@util.cross_origin(methods=["GET"])
@web_util.requires_login(accept_key=False)
def get_ondemand_replay(*, user_id):
    bucket = model.get_ondemand_replay_bucket()
    blob = gcloud_storage.Blob("ondemand_{}".format(user_id), bucket, chunk_size=262144)
    buffer = io.BytesIO()

    try:
        blob.download_to_file(buffer)
    except gcloud_exceptions.NotFound:
        raise util.APIError(404, message="Replay not found.")

    buffer.seek(0)
    response = flask.make_response(flask.send_file(
        buffer,
        mimetype="application/x-halite-3-replay",
        as_attachment=True,
        attachment_filename="{}.hlt".format(user_id)))

    response.headers["Content-Length"] = str(buffer.getbuffer().nbytes)

    return response
