"""
Coordinator API for running games on-demand (e.g. for the web editor
and tutorial).
"""

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
def continue_ondemand():
    pass
