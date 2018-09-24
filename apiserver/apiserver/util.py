import contextlib
import cProfile
import math
import os
import threading
import time
import urllib.parse

import flask
import sqlalchemy
from flask_cors import cross_origin as flask_cross_origin

from . import config


class APIError(Exception):
    """
    Based on http://flask.pocoo.org/docs/0.12/patterns/apierrors/
    """
    def __init__(self, status_code, *, message=None, body=None):
        super().__init__()

        self.status_code = status_code
        self.message = message
        self.body = body

    def to_dict(self):
        result = dict(self.body or ())
        result["status"] = "failure"
        if self.message is not None:
            result["message"] = self.message

        return result


def cross_origin(*args, **kwargs):
    """Mark a view as cross-origin accessible."""
    kwargs["origins"] = config.CORS_ORIGINS
    kwargs["supports_credentials"] = True
    kwargs["allow_headers"] = ["Origin", "Accept", "Content-Type"]
    kwargs["methods"] = ["GET", "POST", "PUT", "OPTIONS", "DELETE"]
    return flask_cross_origin(*args, **kwargs)


@cross_origin(methods=["GET", "POST", "PUT", "OPTIONS", "DELETE"])
def handle_api_error(error):
    """
    The Flask error handler for APIErrors. Use with @app.errorhandler.

    :param error:
    :return:
    """
    response = flask.jsonify(error.to_dict())
    response.status_code = error.status_code
    return response


def tier(rank, total_users):
    """Given a rank (1-based), return the tier name."""
    thresholds = tier_thresholds(total_users)
    for tier, threshold in sorted(thresholds.items(), key=lambda item: item[1]):
        if rank <= threshold:
            return tier

    return config.TIER_4_NAME


def tier_thresholds(total_users):
    """Return the lowest rank that fits each tier."""
    num_players = 0
    result = {}
    for tier, percentage in ((config.TIER_0_NAME, config.TIER_0_PERCENT),
                             (config.TIER_1_NAME, config.TIER_1_PERCENT),
                             (config.TIER_2_NAME, config.TIER_2_PERCENT),
                             (config.TIER_3_NAME, config.TIER_3_PERCENT)):
        num_players += math.ceil(percentage * total_users)
        result[tier] = num_players

    result[config.TIER_4_NAME] = total_users

    return result


def response_success(more=None, status_code=200):
    """
    Build a JSON response for a successful REST call.

    :param more: Additional data to be included in the JSON.
    :param status_code: The status code to use.
    :return: The Flask response (tuple of JSON and status code)
    """
    response = {
        "status": "success",
    }
    if more is not None:
        response.update(more)
    return flask.jsonify(response), status_code


def validate_api_key(api_key):
    """
    Validate the given API key and retrieve the corresponding user record.

    :raises: APIError if the key is invalid
    """
    if not api_key:
        return None

    if ":" not in api_key:
        raise APIError(403, message="API key is in invalid format.")

    # Lazy import to avoid circularity
    from . import model

    user_id, api_key = api_key.split(":", 1)
    user_id = int(user_id)
    with model.read_conn() as conn:
        user = conn.execute(sqlalchemy.sql.select([
            model.users.c.id.label("user_id"),
            model.users.c.is_admin,
            model.users.c.api_key_hash,
            model.users.c.is_email_good,
            model.users.c.is_active,
        ]).where(model.users.c.id == user_id)).first()

        if not user:
            return None

        if config.api_key_context.verify(api_key, user["api_key_hash"]):
            return user

    return None


def build_site_url(page, params, base_url=config.SITE_URL):
    """

    :param page:
    :param params:
    :param base_url:
    :return:
    """

    return "{}?{}".format(
        urllib.parse.urljoin(base_url, page),
        urllib.parse.urlencode(params))


@contextlib.contextmanager
def profiling(name):
    profiler = cProfile.Profile()
    profiler.enable()
    try:
        yield
    finally:
        profiler.disable()
        profiler.dump_stats(name)


def profiled(view):
    def _view(*args, **kwargs):
        with profiling("{}-{}-{}-{}.prof".format(view.__name__,
                                                 time.time(),
                                                 os.getpid(),
                                                 threading.get_ident())):
            return view(*args, **kwargs)
    return _view
