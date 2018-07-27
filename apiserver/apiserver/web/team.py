"""
Team API endpoints - list, create, update teams
"""

import secrets

import flask
import sqlalchemy

from .. import model, util

from . import util as web_util
from .blueprint import web_api


def make_team_record(team, members, show_verification_code=False):
    result = {
        "team_id": team["id"],
        "created": team["created"],
        "name": team["name"],
        "members": {},
        "leader_id": team["leader_id"],
    }

    if show_verification_code:
        result["verification_code"] = team["verification_code"]

    for member in members:
        record = {
            "user_id": member["user_id"],
            "is_leader": member["user_id"] == team["leader_id"],
            "username": member["username"],
            "player_level": member["player_level"],
            "organization_id": member["organization_id"],
            "organization_name": member["organization_name"],
            "country_code": member["country_code"],
            "country_subdivision_code": member["country_subdivision_code"],
        }
        if member["user_id"] == team["leader_id"]:
            result["num_submissions"] = member["num_submissions"]
            result["score"] = member["score"]
            result["mu"] = member["mu"]
            result["sigma"] = member["sigma"]
            result["rank"] = member["rank"]
        result["members"][member["user_id"]] = record

    return result


def get_team_members(conn, team):
    return conn.execute(sqlalchemy.sql.select([
        model.team_members.c.user_id,
        model.all_users.c.username,
        model.all_users.c.player_level,
        model.all_users.c.organization_id,
        model.all_users.c.organization_name,
        model.all_users.c.country_code,
        model.all_users.c.country_subdivision_code,
        model.all_users.c.num_submissions,
        model.all_users.c.score,
        model.all_users.c.mu,
        model.all_users.c.sigma,
        model.all_users.c.rank,
    ]).select_from(model.team_members.join(
        model.all_users,
        model.team_members.c.user_id == model.all_users.c.user_id
    )).where(
        model.team_members.c.team_id == team["id"]
    )).fetchall()


def get_team_helper(team_id, user_id=None):
    with model.read_engine().connect() as conn:
        query = model.teams.select().where(
            model.teams.c.id == team_id
        ).reduce_columns()

        team = conn.execute(query).first()
        if not team:
            raise util.APIError(
                404,
                message="Team {} not found.".format(team_id))

        members = get_team_members(conn, team)
        return make_team_record(team, members,
                                show_verification_code=user_id == team["leader_id"])


def list_teams_helper(offset, limit, participant_clause,
                           where_clause, order_clause):
    with model.read_engine().connect() as conn:
        query = model.teams.select().where(
            where_clause &
            sqlalchemy.sql.exists(model.team_members.select(
                participant_clause &
                (model.teams.c.id == model.team_members.c.team_id)
            ).correlate(model.teams))
        ).order_by(*order_clause).offset(offset).limit(limit).reduce_columns()

        teams = conn.execute(query)
        result = []
        for team in teams.fetchall():
            members = get_team_members(conn, team)
            result.append(make_team_record(team, members))

        return result


@web_api.route("/team", methods=["GET"])
@util.cross_origin(methods=["GET", "POST"])
def list_teams():
    offset, limit = web_util.get_offset_limit()
    where_clause, order_clause, manual_sort = web_util.get_sort_filter({
        "id": model.teams.c.id,
        "created": model.teams.c.created,
        "name": model.teams.c.name,
    }, ["member"])

    participant_clause = sqlalchemy.true()
    for (field, op, val) in manual_sort:
        if field == "participant":
            participant_clause &= op(model.team_members.c.user_id, val)

    result = list_teams_helper(offset, limit,
                               participant_clause,
                               where_clause, order_clause)
    return flask.jsonify(result)


@web_api.route("/team", methods=["POST"])
@util.cross_origin(methods=["GET", "POST"])
@web_util.requires_login()
def create_team(*, user_id):
    if "name" not in flask.request.json:
        raise util.APIError(400, message="Please provide a team name.")

    team_name = flask.request.json["name"]

    # TODO: validate team name

    verification_code = secrets.token_hex(16)

    # Check if user is already on a team
    with model.engine.begin() as conn:
        query = model.team_members.select().where(
            model.team_members.c.user_id == user_id)
        if conn.execute(query).first():
            raise util.APIError(
                400, message="You're already on a team.")

        team_id = conn.execute(model.teams.insert().values(
            name=team_name,
            verification_code=verification_code,
            leader_id=user_id,
        )).inserted_primary_key[0]
        conn.execute(model.team_members.insert().values(
            team_id=team_id,
            user_id=user_id,
        ))

        return util.response_success({
            "team_id": team_id,
            "verification_code": verification_code,
        })


@web_api.route("/team/<int:team_id>", methods=["GET"])
@util.cross_origin(methods=["GET", "POST"])
@web_util.requires_login(optional=True)
def get_team(team_id, *, user_id=None):
    # If user logged in, give them code
    result = get_team_helper(team_id, user_id)
    return flask.jsonify(result)


@web_api.route("/team/<int:team_id>/user", methods=["POST"])
@util.cross_origin(methods=["POST"])
@web_util.requires_login()
def associate_user_team(team_id, *, user_id):
    verification_code = flask.request.form.get("verification_code")
    if not verification_code:
        raise util.APIError(
            400,
            message="Please provide the team's verification code."
        )

    with model.engine.connect() as conn:
        team = conn.execute(model.teams.select(model.teams.c.id == team_id)).first()

        if not team:
            raise util.APIError(404, message="Team {} does not exist.".format(team_id))

        if team["verification_code"] != verification_code:
            raise util.APIError(403, message="Incorrect verification code.")

        members = conn.execute(
            model.team_members.select(model.team_members.c.team_id == team_id)
        ).fetchall()

        if len(members) >= 4:
            raise util.APIError(400, message="Team already has 4 members.")

        for member in members:
            if member["user_id"] == user_id:
                raise util.APIError(400, message="You're already in this team.")

        conn.execute(
            model.team_members.insert().values(
                team_id=team_id,
                user_id=user_id
            ))

        # TODO: delete user's bots/remove them from matchmaking

        return util.response_success()



# TODO: add /bot, /match endpoints that redirect to corresponding
# endpoints for team leader (with HTTP redirect?)
