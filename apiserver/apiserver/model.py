import enum
import random
import time

import google.cloud.datastore as gcloud_datastore
import google.cloud.storage as gcloud_storage
import sqlalchemy

from . import app, config


class CompileStatus(enum.Enum):
    """The compilation status of a bot."""
    # Bot just uploaded
    UPLOADED = "Uploaded"
    # Bot being compiled
    IN_PROGRESS = "InProgress"
    # Bot playing games
    SUCCESSFUL = "Successful"
    # Bot failed to compile
    FAILED = "Failed"
    # Bot not running (e.g. part of team and not leader)
    DISABLED = "Disabled"


class ChallengeStatus(enum.Enum):
    """The status of a challenge."""
    CREATED = "created"
    PLAYING_GAME = "playing_game"
    FINISHED = "finished"


# Database setup
engines = []
for (_, _, port) in config.DATABASES:
    engines.append(sqlalchemy.create_engine(config.DATABASE_URL.format(port=port)))
engine = engines[0]
metadata = sqlalchemy.MetaData(bind=engine)

def read_conn():
    """
    Pick and connect to a random read replica, falling back to the
    master instance if needed.
    """
    try:
        return random.choice(engines[1:]).connect()
    except Exception as e:
        app.logger.error("Could not connect to read replica", exc_info=e)
        return engine.connect()

organizations = sqlalchemy.Table("organization", metadata, autoload=True)
organization_email_domains = \
    sqlalchemy.Table("organization_email_domain", metadata, autoload=True)
users = sqlalchemy.Table("user", metadata, autoload=True)
halite_1_users = sqlalchemy.Table("halite_1_user", metadata, autoload=True)
halite_2_users = sqlalchemy.Table("halite_2_user", metadata, autoload=True)
leagues = sqlalchemy.Table("leagues", metadata, autoload=True)
user_notifications = sqlalchemy.Table("user_notification", metadata, autoload=True)
bots = sqlalchemy.Table("bot", metadata, autoload=True)
bot_history = sqlalchemy.Table("bot_history", metadata, autoload=True)
games = sqlalchemy.Table("game", metadata, autoload=True)
game_view_stats = sqlalchemy.Table("game_view_stat", metadata, autoload=True)
game_participants = sqlalchemy.Table("game_participant", metadata, autoload=True)
hackathons = sqlalchemy.Table("hackathon", metadata, autoload=True)
hackathon_participants = sqlalchemy.Table("hackathon_participant", metadata, autoload=True)
hackathon_snapshot = sqlalchemy.Table("hackathon_snapshot", metadata, autoload=True)
challenges = sqlalchemy.Table("challenge", metadata, autoload=True)
challenge_participants = sqlalchemy.Table("challenge_participant", metadata, autoload=True)
teams = sqlalchemy.Table("team", metadata, autoload=True)

def ranked_bots_query(alias="ranked_bots"):
    """
    Builds a query that ranks all bots.

    This is a function in case you need this as a subquery multiple times.
    """
    return sqlalchemy.sql.select([
        bots.c.user_id,
        bots.c.id.label("bot_id"),
        bots.c.mu,
        bots.c.sigma,
        bots.c.score,
        bots.c.games_played,
        bots.c.version_number,
        bots.c.language,
        bots.c.update_time,
        bots.c.compile_status,
        sqlalchemy.sql.func.rank().over(
            order_by=bots.c.score.desc()
        ).label("bot_rank"),
        sqlalchemy.sql.func.rank().over(
            partition_by=users.c.organization_id,
            order_by=bots.c.score.desc()
        ).label("bot_organization_rank"),
    ]).select_from(
        bots.join(users, bots.c.user_id == users.c.id)
    ).where(
        users.c.is_active == True
    ).order_by(
        bots.c.score.desc()
    ).alias(alias)


def hackathon_ranked_bots_query(hackathon_id,
                                *,
                                variable="hrank",
                                alias="hackathon_ranked_bots"):
    """
    Builds a query that ranks all bots within a given hackathon.
    """

    temptable = sqlalchemy.sql.select([
        hackathon_snapshot.c.user_id,
        hackathon_snapshot.c.bot_id,
        hackathon_snapshot.c.score,
        hackathon_snapshot.c.mu,
        hackathon_snapshot.c.sigma,
        hackathon_snapshot.c.games_played,
        hackathon_snapshot.c.version_number,
        hackathon_snapshot.c.language,
        users.c.is_active,
    ]).select_from(
        hackathon_snapshot.join(users, hackathon_snapshot.c.user_id == users.c.id)
    ).where(
        (hackathon_snapshot.c.hackathon_id == hackathon_id) &
        (users.c.is_active == True)
    ).order_by(hackathon_snapshot.c.score.desc()).alias("temptable")

    return sqlalchemy.sql.select([
        sqlalchemy.sql.func.rank().over(
            order_by=temptable.c.score.desc()
        ).label("local_rank"),
        temptable.c.user_id,
        temptable.c.bot_id,
        temptable.c.mu,
        temptable.c.sigma,
        temptable.c.score,
        temptable.c.games_played,
        temptable.c.version_number,
        temptable.c.language,
    ]).select_from(temptable).alias(alias)


ranked_bots = ranked_bots_query()


_func = sqlalchemy.sql.func
leader_bots = ranked_bots_query("leader_bot")
teams_bots = teams.join(leader_bots, teams.c.leader_id == leader_bots.c.user_id)
duplicate_teams = teams.alias("teams2")
# Summary of all users, regardless of whether they have bots
all_users = sqlalchemy.sql.select([
    users.c.id.label("user_id"),
    users.c.username,
    users.c.player_level,
    users.c.organization_id,
    organizations.c.organization_name,
    users.c.country_code,
    users.c.country_subdivision_code,
    users.c.github_email.label("email"),
    users.c.email.label("personal_email"),
    users.c.is_email_good,
    users.c.is_gpu_enabled,
    users.c.oauth_provider,
    users.c.oauth_profile_image_key,
    duplicate_teams.c.id.label("team_id"),
    duplicate_teams.c.name.label("team_name"),
    duplicate_teams.c.leader_id.label("team_leader_id"),
    _func.count(ranked_bots.c.user_id).label("num_bots"),
    _func.coalesce(
        _func.sum(leader_bots.c.games_played),
        _func.sum(ranked_bots.c.games_played),
        0,
    ).label("num_games"),
    _func.coalesce(
        _func.sum(leader_bots.c.version_number),
        _func.sum(ranked_bots.c.version_number),
        0,
    ).label("num_submissions"),
    _func.coalesce(
        _func.max(leader_bots.c.score),
        _func.max(ranked_bots.c.score),
        0,
    ).label("score"),
    _func.coalesce(
        _func.max(leader_bots.c.sigma),
        _func.max(ranked_bots.c.sigma),
        0,
    ).label("sigma"),
    _func.coalesce(
        _func.max(leader_bots.c.mu),
        _func.max(ranked_bots.c.mu),
        0,
    ).label("mu"),
    _func.coalesce(
        _func.min(leader_bots.c.bot_rank),
        _func.min(ranked_bots.c.bot_rank),
    ).label("rank"),
    _func.coalesce(
        _func.min(leader_bots.c.bot_organization_rank),
        _func.min(ranked_bots.c.bot_organization_rank),
    ).label("organization_rank"),
]).select_from(
    users.join(
        ranked_bots,
        ranked_bots.c.user_id == users.c.id,
        isouter=True,
    ).join(
        organizations,
        organizations.c.id == users.c.organization_id,
        isouter=True
    ).join(
        teams_bots,
        users.c.team_id == teams_bots.c.team_id,
        isouter=True
    ).join(
        duplicate_teams,
        users.c.team_id == duplicate_teams.c.id,
        isouter=True
    )
).where(
    users.c.is_active == True
).group_by(
    users.c.id,
    users.c.username,
    users.c.player_level,
    users.c.organization_id,
    organizations.c.organization_name,
    users.c.country_code,
    users.c.country_subdivision_code,
    users.c.github_email,
    users.c.email,
    users.c.is_email_good,
    users.c.is_gpu_enabled,
    duplicate_teams.c.id,
    duplicate_teams.c.name,
    duplicate_teams.c.leader_id,
).alias("all_users")


# All submitted bots, ranked with user info
ranked_bots_users = sqlalchemy.sql.select([
    users.c.id.label("user_id"),
    users.c.username,
    users.c.oauth_profile_image_key.label("profile_image_key"),
    users.c.player_level,
    users.c.organization_id,
    organizations.c.organization_name,
    users.c.country_code,
    users.c.country_subdivision_code,
    users.c.github_email.label("email"),
    users.c.is_gpu_enabled,
    teams.c.id.label("team_id"),
    teams.c.name.label("team_name"),
    teams.c.leader_id.label("team_leader_id"),
    ranked_bots.c.bot_id,
    ranked_bots.c.games_played.label("num_games"),
    ranked_bots.c.version_number.label("num_submissions"),
    ranked_bots.c.mu,
    ranked_bots.c.sigma,
    ranked_bots.c.score,
    ranked_bots.c.language,
    ranked_bots.c.update_time,
    ranked_bots.c.bot_rank.label("rank"),
    ranked_bots.c.bot_organization_rank.label("organization_rank"),
    ranked_bots.c.compile_status,
]).select_from(
    ranked_bots.join(
        users,
        ranked_bots.c.user_id == users.c.id,
    ).join(
        organizations,
        organizations.c.id == users.c.organization_id,
        isouter=True
    ).join(
        teams,
        users.c.team_id == teams.c.id,
        isouter=True
    )
).alias("ranked_bots_users")


# Users, ranked by their best bot
def ranked_users_query(alias="ranked_users"):
    ranked_bots = ranked_bots_query("rurank")
    return sqlalchemy.sql.select([
        users.c.id.label("user_id"),
        users.c.team_id,
        users.c.username,
        # Perform a no-op operation so we can label the column easily
        ranked_bots.c.bot_rank.label("rank"),
    ]).select_from(
        users.join(ranked_bots, ranked_bots.c.user_id == users.c.id)
    ).group_by(
        users.c.id,
        users.c.team_id,
        users.c.username,
        ranked_bots.c.bot_rank
    ).alias(alias)


# Total number of ranked users that have played a game
total_ranked_users = sqlalchemy.sql.select([
    _func.count(sqlalchemy.distinct(bots.c.user_id))
]).select_from(
    bots.join(users)
).where(
    (bots.c.games_played > 0) &
    (users.c.is_active == True)
)


def hackathon_total_ranked_users_query(hackathon_id):
    """Build a query counting all users in a hackathon."""
    return sqlalchemy.sql.select([
        _func.count(sqlalchemy.distinct(bots.c.user_id))
    ]).select_from(
        bots.join(
            hackathon_participants,
            (bots.c.user_id == hackathon_participants.c.user_id) &
            (hackathon_participants.c.hackathon_id == hackathon_id)
        ).join(
            users,
            (bots.c.user_id == users.c.id) &
            (users.c.is_email_good == True) &
            (users.c.is_active == True)
        )
    ).where(bots.c.games_played > 0)


def hackathon_ranked_bots_users_query(hackathon_id, *, alias="hackathon_ranked_bots_users"):
    """Build a query that ranks all users in a hackathon by their best bot."""
    local_rank = hackathon_ranked_bots_query(hackathon_id, alias="local_rank")
    return sqlalchemy.sql.select([
        users.c.id.label("user_id"),
        users.c.username,
        users.c.player_level,
        users.c.organization_id,
        organizations.c.organization_name,
        users.c.country_code,
        users.c.country_subdivision_code,
        ranked_bots.c.bot_id,
        local_rank.c.games_played.label("num_games"),
        local_rank.c.version_number.label("num_submissions"),
        local_rank.c.mu,
        local_rank.c.score,
        local_rank.c.language,
        ranked_bots.c.update_time,
        local_rank.c.local_rank,
        ranked_bots.c.compile_status,
    ]).select_from(
        ranked_bots.join(
            users,
            (ranked_bots.c.user_id == users.c.id) &
            # Only include verified users
            (users.c.is_email_good == True),
        ).join(
            local_rank,
            (local_rank.c.user_id == ranked_bots.c.user_id) &
            (local_rank.c.bot_id == ranked_bots.c.bot_id)
        ).join(
            organizations,
            organizations.c.id == users.c.organization_id,
            isouter=True
        )
    ).alias(alias)


def team_leader_query(user_id):
    return teams.join(users, teams.c.id == users.c.team_id).select(users.c.id == user_id)


def cached(f):
    """Decorator for nullary functions that caches their result."""
    __cache = None

    def __cached_internal():
        nonlocal __cache

        if __cache is None:
            __cache = f()

        return __cache

    return __cached_internal


@cached
def get_storage_client():
    return gcloud_storage.Client(project=config.GCLOUD_PROJECT)


# If we reconstruct the client each time, that creates a new metadata
# request (to get the service account credentials). These requests
# don't seem to be closed properly (by google-auth?), leading to our
# process running out of fds. Instead, cache the client per process.
@cached
def get_datastore_client():
    return gcloud_datastore.Client(project=config.GCLOUD_PROJECT)


def get_compilation_bucket():
    """Get the object storage bucket for bots to be compiled."""
    return get_storage_client().get_bucket(config.GCLOUD_COMPILATION_BUCKET)


def get_bot_bucket():
    """Get the object storage bucket for compiled bots."""
    return get_storage_client().get_bucket(config.GCLOUD_BOT_BUCKET)


def get_replay_bucket(kind=0):
    """Get the object storage bucket for game replays."""
    return get_storage_client().get_bucket(config.GCLOUD_REPLAY_BUCKETS[kind])


def get_ondemand_replay_bucket():
    """Get object storage bucket for ondemand game replays."""
    return get_storage_client().get_bucket(config.GCLOUD_ONDEMAND_REPLAY_BUCKET)


def get_gym_bot_bucket():
    """Get object storage bucket for pre-built enemy bots."""
    return get_storage_client().get_bucket(config.GCLOUD_GYM_BUCKET)


def get_editor_bucket():
    """Get the object storage bucket for game replays."""
    return get_storage_client().get_bucket(config.GCLOUD_EDITOR_BUCKET)

def get_starter_bucket():
    """Get the object storage bucket for game replays."""
    return get_storage_client().get_bucket(config.GCLOUD_STARTER_BUCKET)

def get_error_log_bucket():
    """Get the object storage bucket for game error log files."""
    return get_storage_client().get_bucket(config.GCLOUD_ERROR_LOG_BUCKET)

def get_worker_log_bucket():
    """Get the object storage bucket for game error log files."""
    return get_storage_client().get_bucket(config.GCLOUD_WORKER_LOG_BUCKET)

def get_deployed_artifacts_bucket():
    """Get the object storage bucket for deployed worker artifacts."""
    return get_storage_client().get_bucket(
        config.GCLOUD_DEPLOYED_ARTIFACTS_BUCKET)


# Log slow queries
@sqlalchemy.event.listens_for(sqlalchemy.engine.Engine, "before_cursor_execute")
def before_cursor_execute(conn, cursor, statement, parameters, context, executemany):
    conn.info.setdefault('query_start_time', []).append(time.time())

@sqlalchemy.event.listens_for(sqlalchemy.engine.Engine, "after_cursor_execute")
def after_cursor_execute(conn, cursor, statement, parameters, context, executemany):
    total = time.time() - conn.info['query_start_time'].pop(-1)
    if total > 1:
        app.logger.warning("Slow query: %f %s %s", total, statement, parameters)
