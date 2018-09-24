"""
Leaderboard API endpoints - get/sort/filter the leaderboard
"""
import collections
import datetime
import operator

import flask
import sqlalchemy

from .. import config, model, util

from . import util as api_util
from .blueprint import web_api

_COUNT_KEY = 'count'
_RELRANK_KEY = 'relrank'
_LEADERBOARD_ALIAS = 'full_leaderboard'


def _count_leaderboard_query(where_clause):
    """
    Considering the desired clause, return the number of distinct users in leaderboard
    :param where_clause: Clause to filter by
    :return: Number of distinct users
    """
    full_leaderboard = model.ranked_bots_users.select().where(where_clause).reduce_columns().alias(_LEADERBOARD_ALIAS)
    return sqlalchemy.sql.select([
        sqlalchemy.sql.func.count(sqlalchemy.distinct(full_leaderboard.c.user_id))
    ]).select_from(full_leaderboard)


@web_api.route("/leaderboard")
@util.cross_origin(methods=["GET"])
def leaderboard():
    result = []
    offset, limit = api_util.get_offset_limit(default_limit=250,
                                              max_limit=10000)

    where_clause, general_clause, order_clause, manual_sort = api_util.get_sort_filter({
        "user_id": model.ranked_bots_users.c.user_id,
        "username": model.ranked_bots_users.c.username,
        "level": model.ranked_bots_users.c.player_level,
        "organization_id": model.ranked_bots_users.c.organization_id,
        "organization_name": model.ranked_bots_users.c.organization_name,
        "country_code": model.ranked_bots_users.c.country_code,
        "country_subdivision_code": model.ranked_bots_users.c.country_subdivision_code,
        "version_number": model.ranked_bots_users.c.num_submissions,
        "num_games": model.ranked_bots_users.c.num_games,
        "rank": model.ranked_bots_users.c.rank,
        "organization_rank": model.ranked_bots_users.c.organization_rank,
        "language": model.ranked_bots_users.c.language,
        "team_name": model.ranked_bots_users.c.team_name,
        "team_id": model.ranked_bots_users.c.team_id,
        "team_leader_id": model.ranked_bots_users.c.team_leader_id,
    }, ["tier"], ["user_id", "username", "rank", "organization_rank", "team_name", "team_id", "team_leader_id"])

    if not order_clause:
        order_clause = [model.ranked_bots_users.c.rank]

    with model.read_conn() as conn:
        if _COUNT_KEY in flask.request.args:
            return str(api_util.get_value(conn.execute(_count_leaderboard_query(where_clause))))

        total_users = api_util.get_value(conn.execute(model.total_ranked_users))

        tier_filter = None
        tier_thresholds = util.tier_thresholds(total_users)
        for (field, op, val) in manual_sort:
            if field == "tier":
                column = model.ranked_bots_users.c.rank
                tier = val
                if val in tier_thresholds:
                    val = tier_thresholds[val]
                else:
                    raise util.APIError(
                        400,
                        message="Tier " + str(val) + " is not recognized.")

                next_tier_dict = {
                    config.TIER_0_NAME: None,
                    config.TIER_1_NAME: tier_thresholds[config.TIER_0_NAME],
                    config.TIER_2_NAME: tier_thresholds[config.TIER_1_NAME],
                    config.TIER_3_NAME: tier_thresholds[config.TIER_2_NAME],
                    config.TIER_4_NAME: tier_thresholds[config.TIER_3_NAME],
                }
                # Negate the filter, since tier and rank are sorted
                # opposite of each other
                if op in (operator.gt, operator.lt, operator.ge, operator.le):
                    if op in (operator.gt, operator.le):
                        val = next_tier_dict.get(tier, val)
                    clause = {
                        operator.gt: operator.le,
                        operator.lt: operator.gt,
                        operator.ge: operator.le,
                        operator.le: operator.gt,
                    }[op](column, val)
                elif op is operator.eq or op is operator.ne:
                    clause = column <= val
                    next_tier = next_tier_dict.get(tier)
                    if next_tier is not None:
                        clause &= column > next_tier

                    if op is operator.ne:
                        clause = ~clause
                else:
                    raise util.APIError(
                        400,
                        message="Comparison operator not supported for tier field.")

                if tier_filter is None:
                    tier_filter = clause
                else:
                    tier_filter |= clause

        if tier_filter is not None:
            where_clause &= tier_filter

        # Only include non-team players/team leaders
        # where_clause &= (
        #     (model.ranked_bots_users.c.team_id == None) |
        #     (model.ranked_bots_users.c.team_id == model.ranked_bots_users.c.user_id))

        if _RELRANK_KEY in flask.request.args:
            query = conn.execute(
                sqlalchemy.sql.select([
                    model.ranked_bots_users.c.user_id,
                    model.ranked_bots_users.c.username,
                    model.ranked_bots_users.c.player_level,
                    model.ranked_bots_users.c.organization_id,
                    model.ranked_bots_users.c.organization_name,
                    model.ranked_bots_users.c.country_code,
                    model.ranked_bots_users.c.country_subdivision_code,
                    model.ranked_bots_users.c.email,
                    model.ranked_bots_users.c.is_gpu_enabled,
                    model.ranked_bots_users.c.team_id,
                    model.ranked_bots_users.c.team_name,
                    model.ranked_bots_users.c.team_leader_id,
                    model.ranked_bots_users.c.bot_id,
                    model.ranked_bots_users.c.num_games,
                    model.ranked_bots_users.c.num_submissions,
                    model.ranked_bots_users.c.mu,
                    model.ranked_bots_users.c.sigma,
                    model.ranked_bots_users.c.score,
                    model.ranked_bots_users.c.language,
                    model.ranked_bots_users.c.update_time,
                    model.ranked_bots_users.c.rank,
                    model.ranked_bots_users.c.organization_rank,
                    model.ranked_bots_users.c.compile_status,
                    sqlalchemy.sql.func.rank().over(
                        partition_by=general_clause,
                        order_by=model.ranked_bots_users.c.score.desc()
                    ).label("relative_rank"),
                ]).select_from(model.ranked_bots_users)
                .where(where_clause).order_by(*order_clause)
                .offset(offset).limit(limit).reduce_columns())
        else:
            query = conn.execute(
                model.ranked_bots_users.select()
                .where(where_clause).order_by(*order_clause)
                .offset(offset).limit(limit).reduce_columns())

        team_ids = []
        for row in query.fetchall():
            user = {
                "user_id": row["user_id"],
                "username": row["username"],
                "level": row["player_level"],
                "organization_id": row["organization_id"],
                "organization": row["organization_name"],
                "version_number": int(row["num_submissions"]),
                "compile_status": row["compile_status"],
                "num_games": int(row["num_games"]),
                "score": float(row["score"]),
                "language": row["language"],
                "country": row["country_code"],
                "rank": int(row["rank"]) if row["rank"] is not None else None,
                "organization_rank": int(row["organization_rank"]) if row["organization_rank"] is not None and row["organization_id"] is not None else None,
                "update_time": row["update_time"],
                "mu": row["mu"],
                "sigma": row["sigma"],
                "team_id": row["team_id"],
                "team_name": row["team_name"],
                "team_leader_id": row["team_leader_id"],
            }

            if "relative_rank" in row:
                user["relative_rank"] = row["relative_rank"]

            if row["team_id"] is not None:
                team_ids.append(row["team_id"])

            if total_users and row["rank"] is not None:
                user["tier"] = util.tier(row["rank"], total_users)
            else:
                user["tier"] = None

            result.append(user)

        team_members = conn.execute(model.all_users.select(
            model.all_users.c.team_id.in_(team_ids)
        ).reduce_columns())
        team_map = collections.defaultdict(list)
        for team_member in team_members.fetchall():
            team_map[team_member["team_id"]].append(team_member)

        for row in result:
            if row["team_id"] in team_map:
                row["team_members"] = [{
                    "user_id": tm["user_id"],
                    "username": tm["username"],
                    "player_level": tm["player_level"],
                    "organization_id": tm["organization_id"],
                    "organization": tm["organization_name"],
                    "country": tm["country_code"],
                } for tm in team_map[row["team_id"]]]
            else:
                row["team_members"] = []

    return flask.jsonify(result)


@web_api.route("/leagues")
@util.cross_origin(methods=["GET"])
def leagues():
    result = []
    with model.read_conn() as conn:
        query = conn.execute(
            model.leagues.select())

        for row in query.fetchall():
            league = {
                "id": row["id"],
                "category": row["category"],
                "name": row["name"],
                "description": row["description"],
                "query": row["query"],
            }

            result.append(league)

    return flask.jsonify(result)


@web_api.route("/leaderboard/organization")
@util.cross_origin(methods=["GET"])
def organization_leaderboard():
    result = []

    organization_wins = sqlalchemy.sql.select([
        model.organizations.c.id.label("organization_id"),
        model.organizations.c.organization_name,
        sqlalchemy.sql.func.count().label("weekly_games_won"),
    ]).select_from(model.games.join(
        model.game_participants,
        (model.games.c.id == model.game_participants.c.game_id) &
        (model.game_participants.c.rank == 1) &
        (model.games.c.time_played >= sqlalchemy.sql.func.current_timestamp() - sqlalchemy.sql.text("interval '7' day"))
    ).join(
        model.users,
        model.game_participants.c.user_id == model.users.c.id
    ).join(
        model.organizations,
        model.users.c.organization_id == model.organizations.c.id
    )).group_by(
        model.organizations.c.id,
        model.organizations.c.organization_name,
    ).alias("organization_wins")

    organization_ranks = sqlalchemy.sql.select([
        organization_wins.c.organization_id,
        organization_wins.c.organization_name,
        organization_wins.c.weekly_games_won,
        sqlalchemy.sql.func.rank().over(
            order_by=organization_wins.c.weekly_games_won.desc()
        ).label("organization_rank"),
    ]).select_from(organization_wins).alias("organization_ranks")

    offset, limit = api_util.get_offset_limit(default_limit=250,
                                              max_limit=10000)

    where_clause, order_clause, manual_sort = api_util.get_sort_filter({
        "organization_id": organization_ranks.c.organization_id,
        "organization_name": organization_ranks.c.organization_name,
        "organization_rank": organization_ranks.c.organization_rank,
    }, ["tier"])

    if not order_clause:
        order_clause = [organization_ranks.c.organization_rank]

    with model.read_conn() as conn:
        total_orgs = api_util.get_value(conn.execute(sqlalchemy.sql.select([
            sqlalchemy.sql.func.count()
        ]).select_from(organization_wins)))

        query = conn.execute(organization_ranks.select()
                             .where(where_clause)
                             .order_by(*order_clause)
                             .offset(offset).limit(limit)).fetchall()

        for row in query:
            org = {
                "organization_id": row["organization_id"],
                "organization_name": row["organization_name"],
                "organization_rank": row["organization_rank"],
                "weekly_games_won": row["weekly_games_won"],
            }
            if total_orgs and org["organization_rank"]:
                org["tier"] = util.tier(org["organization_rank"], total_orgs)
            result.append(org)

    return flask.jsonify(result)
