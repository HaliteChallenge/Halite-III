import datetime
import functools
import operator

import arrow
import flask
import sqlalchemy

from .. import config, model, util

def validate_api_key(api_key):
    """
    Validate the given API key and retrieve the corresponding user record.

    :raises: util.APIError if the key is invalid
    """
    if not api_key:
        return None

    if ":" not in api_key:
        raise util.APIError(403, message="API key is in invalid format.")

    user_id, api_key = api_key.split(":", 1)
    user_id = int(user_id)
    with model.engine.connect() as conn:
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


def validate_session_cookie(user_id):
    """
    Validate the session cookie and retrieve the corresponding user record.
    """
    with model.engine.connect() as conn:
        user = conn.execute(sqlalchemy.sql.select([
            model.users.c.id.label("user_id"),
            model.users.c.is_admin,
            model.users.c.api_key_hash,
            model.users.c.is_email_good,
            model.users.c.is_active,
        ]).where(model.users.c.id == user_id)).first()

        return user


def requires_login(accept_key=False, optional=False, admin=False,
                   association=False):
    """
    Indicates that an endpoint requires the user to be logged in.

    :param accept_key: if True, then accept an API key, otherwise only accept
    a session cookie (OAuth).
    :param optional: if True, do not return HTTP 403 if the user is not
    logged in.
    :param admin: if True, only accept admin users.
    :param association: if True, only accept users that have associated and
    verified their email.
    """
    def _requires_login(view):
        @functools.wraps(view)
        def decorated_view(*args, **kwargs):
            user = None
            if accept_key:
                user = validate_api_key(
                    flask.request.headers.get(config.API_KEY_HEADER))

            if not user:
                user = validate_session_cookie(
                    flask.session.get(config.SESSION_COOKIE))

            if user:
                if admin and user["is_admin"]:
                    kwargs["user_id"] = user["user_id"]
                elif admin:
                    raise util.APIError(
                        403, message="User cannot take this action.")
                else:
                    kwargs["user_id"] = user["user_id"]
            elif optional:
                kwargs["user_id"] = None
            else:
                raise util.APIError(403, message="User not logged in.")

            return view(*args, **kwargs)

        return decorated_view

    return _requires_login


def requires_competition_open(view):
    """Indicates that an endpoint requires the competition is running."""
    @functools.wraps(view)
    def decorated_view(*args, **kwargs):
        if not config.COMPETITION_OPEN:
            raise util.APIError(
                403,
                message="Sorry, the competition has ended. Thanks for playing!")
        return view(*args, **kwargs)

    return decorated_view


def is_user_admin(user_id, *, conn):
    user = conn.execute(model.users.select(model.users.c.id == user_id)).first()
    return user and user["is_admin"]

