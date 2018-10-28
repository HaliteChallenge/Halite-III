import datetime
import functools
import operator

import arrow
import flask
import sqlalchemy
import pycountry

from .. import config, model, util


def no_cache(response):
    """Add headers to inhibit caching on a response."""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Pragma"] = "no-cache"
    response.headers["Expires"] = "0"
    response.headers["Cache-Control"] = "public, max-age=0"

    return response


def validate_country(country_code, subdivision_code):
    try:
        country = pycountry.countries.get(alpha_3=country_code)
    except KeyError:
        return False

    if subdivision_code:
        subdivisions = pycountry.subdivisions.get(country_code=country.alpha_2)
        for subdivision in subdivisions:
            if subdivision.code == subdivision_code:
                return True
        return False
    else:
        return True


def validate_user_level(level):
    return level in ('High School', 'University',
                     'Professional')


def validate_session_cookie(user_id, session_secret):
    """
    Validate the session cookie and retrieve the corresponding user record.
    """
    with model.read_conn() as conn:
        user = conn.execute(sqlalchemy.sql.select([
            model.users.c.id.label("user_id"),
            model.users.c.is_admin,
            model.users.c.api_key_hash,
            model.users.c.is_email_good,
            model.users.c.is_active,
            model.users.c.session_secret,
        ]).where((model.users.c.id == user_id) &
                 (model.users.c.session_secret == session_secret))).first()

        return user


def requires_login(accept_key=False, optional=False, admin=False,
                   association=False, maybe_admin=False):
    """
    Indicates that an endpoint requires the user to be logged in.

    :param accept_key: if True, then accept an API key, otherwise only accept
    a session cookie (OAuth).
    :param optional: if True, do not return HTTP 403 if the user is not
    logged in.
    :param admin: if True, only accept admin users.
    :param maybe_admin: if True, accept admin users and pass the `is_admin`
    flag to the view.
    :param association: if True, only accept users that have associated and
    verified their email.
    """
    def _requires_login(view):
        @functools.wraps(view)
        def decorated_view(*args, **kwargs):
            user = None
            if accept_key:
                user = util.validate_api_key(
                    flask.request.headers.get(config.API_KEY_HEADER))

            if not user:
                user = validate_session_cookie(
                    flask.session.get(config.SESSION_COOKIE),
                    flask.session.get(config.SESSION_SECRET))

            if user:
                if not user["is_active"]:
                    raise util.APIError(
                        403, message="User is disabled.")
                if admin and user["is_admin"]:
                    kwargs["user_id"] = user["user_id"]
                elif admin:
                    raise util.APIError(
                        403, message="User cannot take this action.")
                else:
                    kwargs["user_id"] = user["user_id"]
                    if maybe_admin:
                        kwargs["is_admin"] = user["is_admin"]
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


def user_mismatch_error(message="Cannot perform action for other user."):
    """
    Error for when the logged in and target users of an action don't match.
    """
    raise util.APIError(400, message=message)


def get_offset_limit(*, default_limit=50, max_limit=250):
    """Get an offset/limit from the query string (or default)."""
    offset = int(flask.request.values.get("offset", 0))
    offset = max(offset, 0)
    limit = int(flask.request.values.get("limit", default_limit))
    limit = min(max(limit, 0), max_limit)

    return offset, limit


def operator_like(field, value):
    return field.like("%{}%".format(value))


def parse_filter(filter_string):
    """
    Parse a filter string into a field name, comparator, and value.
    :param filter_string: Of the format field,operator,value.
    :return: (field_name, operator_func, value)
    """
    try:
        field, cmp, value = filter_string.split(",", 2)
    except Exception as e:
        raise util.APIError(
            400,
            message="Filter '{}' is ill-formed.".format(filter_string))

    operation = {
        "=": operator.eq,
        "<": operator.lt,
        "<=": operator.le,
        ">": operator.gt,
        ">=": operator.ge,
        "!=": operator.ne,
        "contains": operator_like,
    }.get(cmp, None)
    if operation is None:
        raise util.APIError(
            400, message="Cannot compare '{}' by '{}'".format(field, cmp))

    return field, operation, value


def int_or_none(x):
    """
    A helper to allow filtering by an integer value or None.
    """
    if x.lower() in ("null", "none", ""):
        return None
    return int(x)


def get_sort_filter(fields, false_fields=(), drilldown_fields=()):
    """
    Parse flask.request to create clauses for SQLAlchemy's order_by and where.

    :param fields: A dictionary of field names to SQLAlchemy table columns
    listing what fields can be sorted/ordered on.
    :param false_fields: A list of fields that can be sorted/ordered on, but
    that the caller will manually handle. (Non-recognized fields generate
    an API error.)
    :param drilldown_fields: If given, also return a where_clause
    without these fields.
    :return: A 2-tuple of (where_clause, order_clause). order_clause is an
    ordered list of columns.
    """
    where_clause = sqlalchemy.true()
    drilldown_clause = sqlalchemy.true()
    order_clause = []
    manual_fields = []

    # Clauses organized by field: clause
    filter_clauses = {}

    for filter_param in flask.request.args.getlist("filter"):
        field, operation, value = parse_filter(filter_param)

        if field not in fields and field not in false_fields:
            raise util.APIError(
                400, message="Cannot filter on field {}".format(field))

        if field in false_fields:
            manual_fields.append((field, operation, value))
            continue

        column = fields[field]
        if isinstance(column.type, sqlalchemy.types.Integer):
            conversion = int_or_none
        elif isinstance(column.type, sqlalchemy.types.DateTime):
            conversion = lambda x: arrow.get(x).datetime
        elif isinstance(column.type, sqlalchemy.types.Float):
            conversion = float
        elif isinstance(column.type, sqlalchemy.types.String):
            conversion = lambda x: x
        else:
            raise util.APIError(
                501,
                message="Filtering on column is not supported yet: " + repr(column))

        try:
            value = conversion(value)
        except Exception as e:
            raise util.APIError(
                401,
                message="Could not convert value {} for filter on '{}'.".format(repr(value), field))

        clause = operation(column, value)
        if field in filter_clauses:
            filter_clauses[field] |= clause
        else:
            filter_clauses[field] = clause

    for field, clause in filter_clauses.items():
        where_clause &= clause
        if field not in drilldown_fields:
            drilldown_clause &= clause

    for order_param in flask.request.args.getlist("order_by"):
        direction = "asc"
        if "," in order_param:
            direction, field = order_param.split(",")
        else:
            field = order_param

        if field not in fields:
            raise util.APIError(
                400, message="Cannot order on field {}".format(field))

        column = fields[field]
        if direction == "asc":
            column = column.asc()
        elif direction == "desc":
            column = column.desc()
        else:
            raise util.APIError(
                400, message="Cannot order column by '{}'".format(direction))

        order_clause.append(column)

    if drilldown_fields:
        return where_clause, drilldown_clause, order_clause, manual_fields
    return where_clause, order_clause, manual_fields


def hackathon_status(start_date, end_date):
    """
    Return the status of the hackathon based on its start/end dates.

    `end_date` may be null (for ongoing hackathons).
    """
    status = "open"
    if end_date and end_date < arrow.now():
        status = "closed"
    elif start_date > arrow.now():
        status = "upcoming"
    return status


def get_value(query):
    """
    Returns the single value item from an executed query
    :param query: The query executed
    :return: The value within
    """
    return query.first()[0]
