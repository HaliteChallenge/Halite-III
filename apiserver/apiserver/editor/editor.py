import io

import flask
import sqlalchemy
import google.cloud.storage as gcloud_storage

from .. import model, util

from . import util as api_util
from .blueprint import editor_api


'''TODO  from user_bot.py
    if user_id != intended_user:
        raise api_util.user_mismatch_error(
            message="Cannot upload bot for another user.")

'''

@editor_api.route("/user/<string:intended_user>/file", methods=["GET"])
@util.cross_origin(methods=["GET"])
@api_util.requires_login(accept_key=True, association=True)
@api_util.requires_competition_open
def list_user_files(intended_user):
    bucket = model.get_editor_bucket()
    return flask.jsonify([a.name[len(intended_user)+1:] for a in bucket.list_blobs(prefix=intended_user) if a.name[:-1] != intended_user])


@editor_api.route("/user/<string:intended_user>/file/<string:file_id>", methods=["GET"])
@util.cross_origin(methods=["GET"])
@api_util.requires_login(accept_key=True, association=True)
@api_util.requires_competition_open
def get_user_file(intended_user, file_id):
    bucket = model.get_editor_bucket()

    blob = gcloud_storage.Blob('%s/%s' % (intended_user, file_id), bucket, chunk_size=262144)
    buffer = io.BytesIO()
    blob.download_to_file(buffer)
    buffer.seek(0)
    response = flask.make_response(flask.send_file(
        buffer,
        mimetype="application/x-halite-2-file",
        as_attachment=True,
        attachment_filename=file_id))

    response.headers["Content-Length"] = str(buffer.getbuffer().nbytes)

    return response

@editor_api.route("/user/<string:intended_user>/file", methods=["POST"])
@util.cross_origin(methods=["POST"])
@api_util.requires_login(accept_key=True, association=True)
@api_util.requires_competition_open
def create_user_filespace(intended_user):
    pass

@editor_api.route("/user/<string:intended_user>/file/<string:file_id>", methods=["POST"])
@util.cross_origin(methods=["POST"])
@api_util.requires_login(accept_key=True, association=True)
@api_util.requires_competition_open
def change_user_file(intended_user, file_id):
    bucket = model.get_editor_bucket()

    blob = gcloud_storage.Blob('%s/%s' % (intended_user, file_id), bucket, chunk_size=262144)
    uploaded_file = validate_bot_submission()
    blob.upload_from_file(uploaded_file)

    return util.response_success()

def validate_file_submission():
    """Validate the uploaded file, returning the file if so."""
    if "uploadFile" not in flask.request.files:
        raise util.APIError(400, message="file not provided (must "
                                         "provide as uploadFile).")

    # Save to GCloud
    uploaded_file = flask.request.files["botFile"]
    if not zipfile.is_zipfile(uploaded_file):
        raise util.APIError(
            400,
            message="file does not appear to be a zip file.")

    uploaded_file.seek(0)
    return uploaded_file

