"""
Team API endpoints - list, create, update teams
"""
import datetime

import flask
import sqlalchemy

from .. import model, util

from . import util as web_util
from .blueprint import web_api


def make_team_record(team, members):
    result = {
        "team_id": team["id"],
        "created": team["created"],
        "name": team["name"],
        "members": {},
        "leader_id": team["leader_id"],
    }

    for member in members:
        record = {
            "user_id": member["user_id"],
            "is_leader": member["is_leader"],
            "username": member["username"],
            "player_level": member["player_level"],
            "organization_id": member["organization_id"],
            "country_code": member["country_code"],
            "country_subdivision_code": member["country_subdivision_code"],
        }
        if member["user_id"] == team["leader_id"]:
            result["num_submissions"] = member["num_submissions"]
            result["score"] = member["score"]
            result["mu"] = member["mu"]
            result["sigma"] = member["sigma"]
        result["members"][member["user_id"]] = record

    return result


def get_team_helper(team_id):
    with model.read_engine().connect() as conn:
        query = sqlalchemy.sql.select([
        ]).select_from(model.teams).where(
            model.challenges.c.id == challenge_id
        ).reduce_columns()

        challenge = conn.execute(query).first()
        if not challenge:
            raise util.APIError(
                404,
                message="Challenge {} not found.".format(challenge_id))

        participants = conn.execute(
            model.challenge_participants.join(
                model.users,
                model.challenge_participants.c.user_id == model.users.c.id
            ).select(
                model.challenge_participants.c.challenge_id == challenge["id"]
            )
        )
        return make_challenge_record(challenge, participants)


def list_teams_helper(offset, limit, participant_clause,
                           where_clause, order_clause):
    with model.read_engine().connect() as conn:
        query = sqlalchemy.sql.select([
            model.teams.c.id,
            model.teams.c.created,
            model.teams.c.name,
        ]).select_from(model.teams).where(
            where_clause &
            sqlalchemy.sql.exists(model.team_members.select(
                participant_clause &
                (model.teams.c.id == model.team_members.c.team_id)
            ).correlate(model.teams))
        ).order_by(*order_clause).offset(offset).limit(limit).reduce_columns()

        teams = conn.execute(query)
        result = []
        for team in teams.fetchall():
            # TODO: grab rank of the team leader
            members = conn.execute(sqlalchemy.sql.select([
                model.team_members.c.user_id,
                model.team_members.c.is_leader,
                model.users.c.username,
                model.users.c.player_level,
                # TODO: organization name
                model.users.c.organization_id,
                model.users.c.country_code,
                model.users.c.country_subdivision_code,
                model.users.c.num_submissions,
                model.users.c.score,
                model.users.c.mu,
                model.users.c.sigma,
            ]).select_from(model.challenge_participants.join(
                model.users,
                model.team_members.c.user_id == model.users.c.id
            )).where(
                model.team_members.c.team_id == team["id"]
            )).fetchall()

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
def create_team():
    pass


@web_api.route("/team/<int:team_id>", methods=["GET"])
@util.cross_origin(methods=["GET", "POST"])
def get_team(team_id):
    result = get_team_helper(team_id)
    return flask.jsonify(result)


@web_api.route("/team/<int:team_id>", methods=["POST"])
@util.cross_origin(methods=["GET", "POST"])
@web_util.requires_login()
def update_team(team_id, *, user_id):
    pass


# TODO: add /bot, /match endpoints that redirect to corresponding
# endpoints for team leader (with HTTP redirect?)
