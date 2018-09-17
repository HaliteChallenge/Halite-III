"""
User bot API endpoints - create/update/delete/list user's bots
"""
import io
import zipfile

import flask
import sqlalchemy

import google.cloud.storage as gcloud_storage
import google.cloud.exceptions as gcloud_exceptions

from .. import model, util

from . import util as api_util
from .blueprint import web_api


@web_api.route("/user/<int:user_id>/bot", methods=["GET"])
@util.cross_origin(methods=["GET"])
def list_user_bots(user_id):
    result = []
    with model.read_conn() as conn:
        bots = conn.execute(sqlalchemy.sql.select([
            model.bots.c.id,
            model.bots.c.version_number,
            model.bots.c.games_played,
            model.bots.c.language,
            model.bots.c.mu,
            model.bots.c.sigma,
            model.bots.c.score,
            model.bots.c.compile_status,
            sqlalchemy.sql.text("ranked_bots.bot_rank"),
        ]).select_from(
            model.bots.join(
                model.ranked_bots,
                (model.bots.c.id == model.ranked_bots.c.bot_id) &
                (model.bots.c.user_id == model.ranked_bots.c.user_id)
            )
        ).where(
            model.bots.c.user_id == user_id
        ).order_by(model.bots.c.id)).fetchall()

        for bot in bots:
            result.append({
                "bot_id": bot["id"],
                "rank": int(bot["bot_rank"]) if bot["bot_rank"] else None,
                "version_number": bot["version_number"],
                "games_played": bot["games_played"],
                "language": bot["language"],
                "mu": bot["mu"],
                "sigma": bot["sigma"],
                "score": bot["score"],
                "compilation_status": bot["compile_status"],
            })

    return flask.jsonify(result)


@web_api.route("/user/<int:intended_user>/bot/<int:bot_id>", methods=["GET"])
@util.cross_origin(methods=["GET", "PUT"])
@api_util.requires_login(accept_key=True, optional=True)
def get_user_bot(intended_user, bot_id, *, user_id):
    with model.read_conn() as conn:
        bot = conn.execute(sqlalchemy.sql.select([
            model.bots.c.id,
            model.bots.c.version_number,
            model.bots.c.games_played,
            model.bots.c.language,
            model.bots.c.score,
            model.bots.c.compile_status,
            sqlalchemy.sql.text("ranked_bots.bot_rank"),
        ]).select_from(
            model.bots.join(
                model.ranked_bots,
                (model.bots.c.id == model.ranked_bots.c.bot_id) &
                (model.bots.c.user_id == model.ranked_bots.c.user_id)
            )
        ).where(
            (model.bots.c.user_id == intended_user) &
            (model.bots.c.id == bot_id)
        ).order_by(model.bots.c.id)).first()

        if not bot:
            raise util.APIError(404, message="Bot not found.")

        mime_type = flask.request.accept_mimetypes.best_match([
            "application/json",
            "text/json",
            "application/zip",
        ])
        if mime_type == "application/zip":
            team = conn.execute(model.team_leader_query(user_id)).first()
            if not user_id:
                raise util.APIError(403, message="You must sign in to download your bot.")
            elif user_id != intended_user and \
                 (not team or team["leader_id"] != intended_user):
                raise api_util.user_mismatch_error(
                    message="Cannot download bot for another user.")

            # Return bot ZIP file
            bucket = model.get_compilation_bucket()
            botname = "{}_{}".format(intended_user, bot_id)
            try:
                blob = bucket.get_blob(botname)
                buffer = io.BytesIO()
                blob.download_to_file(buffer)
                buffer.seek(0)
                response = flask.make_response(flask.send_file(
                    buffer,
                    mimetype="application/zip",
                    as_attachment=True,
                    attachment_filename=botname + ".zip"))
                response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
                response.headers["Pragma"] = "no-cache"
                response.headers["Expires"] = "0"
                response.headers["Cache-Control"] = "public, max-age=0"
                return response
            except gcloud_exceptions.NotFound:
                raise util.APIError(404, message="Bot not found.")


        return flask.jsonify({
            "bot_id": bot_id,
            "rank": int(bot["bot_rank"]) if bot["bot_rank"] else None,
            "version_number": bot["version_number"],
            "games_played": bot["games_played"],
            "language": bot["language"],
            "score": bot["score"],
            "compilation_status": bot["compile_status"],
        })


def validate_bot_submission():
    """Validate the uploaded bot, returning the bot file if so."""
    if "botFile" not in flask.request.files:
        raise util.APIError(400, message="Bot file not provided (must "
                                         "provide as botFile).")

    # Save to GCloud
    uploaded_file = flask.request.files["botFile"]
    if not zipfile.is_zipfile(uploaded_file):
        raise util.APIError(
            400,
            message="Bot file does not appear to be a zip file. Please "
                    "upload a zip file containing your bot, where the "
                    "main file is named MyBot with an appropriate "
                    "extension.")

    uploaded_file.seek(0)
    return uploaded_file


@web_api.route("/user/<int:intended_user>/bot", methods=["POST"])
@util.cross_origin(methods=["POST"])
@api_util.requires_login(accept_key=True, association=True)
@api_util.requires_competition_open
def create_user_bot(intended_user, *, user_id):
    if user_id != intended_user:
        raise api_util.user_mismatch_error(
            message="Cannot create bot for another user.")

    _ = validate_bot_submission()

    with model.engine.connect() as conn:
        # If user in team, allow creation as team leader bot
        team = conn.execute(model.team_leader_query(user_id)).first()
        target_user_id = user_id
        if team:
            target_user_id = team["leader_id"]

        current_bot = conn.execute(
            model.bots.select(model.bots.c.user_id == target_user_id)).first()

        if current_bot:
            raise util.APIError(
                400, message="Only one bot allowed per user/team.")

        conn.execute(model.bots.insert().values(
            user_id=target_user_id,
            id=0,
            compile_status=model.CompileStatus.DISABLED.value,
        ))

    store_user_bot(intended_user=intended_user, user_id=user_id, bot_id=0)
    return util.response_success({
        "bot_id": 0,
    }, status_code=201)


@web_api.route("/user/<int:intended_user>/bot/<int:bot_id>", methods=["PUT"])
@util.cross_origin(methods=["GET", "PUT"])
@api_util.requires_login(accept_key=True, association=True)
@api_util.requires_competition_open
def store_user_bot(user_id, intended_user, bot_id):
    """Store an uploaded bot in object storage."""
    if user_id != intended_user:
        raise api_util.user_mismatch_error(
            message="Cannot upload bot for another user.")

    if bot_id != 0:
        raise util.APIError(
            400, message="Sorry, only one bot allowed per user.")

    uploaded_file = validate_bot_submission()

    with model.engine.connect() as conn:
        team = conn.execute(model.team_leader_query(user_id)).first()
        if team:
            user_id = intended_user = team["leader_id"]

        bot_where_clause = (model.bots.c.user_id == user_id) & \
            (model.bots.c.id == bot_id)
        bot = conn.execute(model.bots.select(bot_where_clause)).first()
        if not bot:
            raise util.APIError(404, message="Bot not found.")

        # Check if the user already has a bot compiling
        if bot["compile_status"] == model.CompileStatus.IN_PROGRESS.value:
            raise util.APIError(400, message="Cannot upload new bot until "
                                             "previous one is compiled.")

        blob = gcloud_storage.Blob("{}_{}".format(user_id, bot_id),
                                   model.get_compilation_bucket(),
                                   chunk_size=262144)
        blob.upload_from_file(uploaded_file)

        # Flag the user as compiling
        update = model.bots.update() \
            .where(bot_where_clause) \
            .values(
                compile_status=model.CompileStatus.UPLOADED.value,
                update_time=sqlalchemy.sql.func.now(),
                timeout_sent=False,
            )
        conn.execute(update)

    return util.response_success({
        "user_id": user_id,
        "bot_id": bot["id"],
    })


@web_api.route("/user/<int:intended_user>/bot/<int:bot_id>", methods=["DELETE"])
@api_util.requires_login(accept_key=True, admin=True)
@api_util.requires_competition_open
def delete_user_bot(intended_user, bot_id, *, user_id):
    if user_id != intended_user:
        raise api_util.user_mismatch_error(
            message="Cannot delete bot for another user.")

    with model.engine.connect() as conn:
        conn.execute(model.bots.delete().where(
            (model.bots.c.user_id == user_id) &
            (model.bots.c.id == bot_id)
        ))

        for bucket in [model.get_bot_bucket(), model.get_compilation_bucket()]:
            try:
                blob = gcloud_storage.Blob(str(user_id), bucket)
                blob.delete()
            except gcloud_exceptions.NotFound:
                pass

        return util.response_success()


@web_api.route("/user/<int:intended_user>/bot/<int:bot_id>/error_log", methods=["GET"])
@util.cross_origin(methods=["GET"])
@api_util.requires_login(accept_key=True)
def get_user_bot_compile_log(intended_user, bot_id, *, user_id):
    if inteded_user != user_id:
        raise api_util.user_mismatch_error(
            message="Cannot get bot compile log for another user.")

    with model.read_conn() as conn:
        bot = conn.execute(sqlalchemy.sql.select([
            model.bots.c.id,
        ]).select_from(model.bots).where(
            (model.bots.c.user_id == intended_user) &
            (model.bots.c.id == bot_id)
        )).first()

        if not bot:
            raise util.APIError(404, message="Bot not found.")

        bucket = model.get_error_log_bucket()
        log_file = "compilation_{}_{}.log".format(intended_user, bot_id)
        try:
            blob = bucket.get_blob(log_file)
            buffer = io.BytesIO()
            blob.download_to_file(buffer)
            buffer.seek(0)
            response = flask.make_response(flask.send_file(
                buffer,
                mimetype="text/plain",
                as_attachment=True,
                attachment_filename=log_file))
            response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
            response.headers["Pragma"] = "no-cache"
            response.headers["Expires"] = "0"
            response.headers["Cache-Control"] = "public, max-age=0"
            return response
        except gcloud_exceptions.NotFound:
            raise util.APIError(404, message="Bot error log not found.")
