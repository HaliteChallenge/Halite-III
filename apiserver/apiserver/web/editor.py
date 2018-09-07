import io
import os

import flask
import sqlalchemy
import google.cloud.storage as gcloud_storage

from .. import model, util

from . import util as api_util
from .blueprint import web_api

# Get a list of users file names
@web_api.route("/editor/<int:intended_user>", methods=["GET"])
@util.cross_origin(methods=["GET"])
@api_util.requires_login(accept_key=True, association=True)
def list_user_files(intended_user, *, user_id):
    if user_id != intended_user:
        raise api_util.user_mismatch_error(
            message="Cannot list files for another user.")

    editor_bucket = model.get_editor_bucket()

    files = []
    directory = "{}/".format(intended_user)
    for file_blob in editor_bucket.list_blobs(prefix=directory):
        # Don't list root directory
        if file_blob.name[:-1] != str(intended_user):
            files.append(file_blob.name[len(directory):])

    return flask.jsonify(files)


@web_api.route("/editor/<int:intended_user>/file/<path:file_id>", methods=["GET"])
@util.cross_origin(methods=["GET"])
@api_util.requires_login(accept_key=True, association=True)
def get_user_file(intended_user, file_id, *, user_id):
    if user_id != intended_user:
        raise api_util.user_mismatch_error(
            message="Cannot list files for another user.")
    bucket = model.get_editor_bucket()

    buffer = io.BytesIO()

    # Google throws an error for 0 byte files, we circumvent this
    try:
        blob = gcloud_storage.Blob('%s/%s' % (intended_user, file_id), bucket, chunk_size=262144)
        blob.download_to_file(buffer)
    except:
        pass

    buffer.seek(0)
    response = flask.make_response(flask.send_file(
        buffer,
        mimetype="application/x-halite-2-file",
        as_attachment=True,
        attachment_filename=file_id))

    response.headers["Content-Length"] = str(buffer.getbuffer().nbytes)

    return response

@web_api.route("/editor/<int:intended_user>/<string:language>", methods=["POST"])
@util.cross_origin(methods=["POST"])
@api_util.requires_login(accept_key=True, association=True)
def create_user_filespace(intended_user, language, *, user_id):
    if user_id != intended_user:
        raise api_util.user_mismatch_error(
            message="Cannot list files for another user.")

    editor_bucket = model.get_editor_bucket()
    user_prefix = "{}/".format(intended_user)
    language_prefix = "{}/".format(language)
    if len([a for a in editor_bucket.list_blobs(prefix=user_prefix)]) == 0:
        starter_bucket = model.get_starter_bucket()
        sub_blobs = [b for b in starter_bucket.list_blobs(prefix=language_prefix)]
        if len(sub_blobs) == 0:
            raise util.APIError(400, message='Selected language is unavailable')
        for sub_blob in sub_blobs:
            starter_bucket.copy_blob(sub_blob,
                    editor_bucket,
                    '%s/%s' % (str(intended_user), '/'.join(sub_blob.name.split('/')[1:])))
        return flask.jsonify(['/'.join(sub_blob.name.split('/')[1:]) for sub_blob in sub_blobs])
    raise util.APIError(400, message='User workspace already created')

@web_api.route("/editor/<int:intended_user>/file/<path:file_id>", methods=["POST"])
@util.cross_origin(methods=["POST"])
@api_util.requires_login(accept_key=True, association=True)
def change_user_file(intended_user, file_id, *, user_id):
    if user_id != intended_user:
        raise api_util.user_mismatch_error(
            message="Cannot list files for another user.")

    bucket = model.get_editor_bucket()

    blob = gcloud_storage.Blob('%s/%s' % (intended_user, file_id), bucket, chunk_size=262144)
    uploaded_file = validate_file_submission()
    blob.upload_from_file(uploaded_file)

    return util.response_success()

def validate_file_submission():
    """Validate the uploaded file, returning the file if so."""
    if "sourceFile" not in flask.request.files:
        raise util.APIError(400, message="file not provided (must "
                                         "provide as uploadFile).")

    # Save to GCloud
    uploaded_file = flask.request.files["sourceFile"]
    uploaded_file.seek(0)
    return uploaded_file


@web_api.route("/editor/<int:intended_user>/file/<path:file_id>", methods=["DELETE"])
@util.cross_origin(methods=["DELETE"])
@api_util.requires_login(accept_key=True, association=True)
def delete_user_file(intended_user, file_id, *, user_id):
    if user_id != intended_user:
        raise api_util.user_mismatch_error(
            message="Cannot list files for another user.")

    bucket = model.get_editor_bucket()

    blob = gcloud_storage.Blob('%s/%s' % (intended_user, file_id), bucket, chunk_size=262144)
    blob.delete()

    return util.response_success()


@web_api.route("/editor/opponents", methods=["GET"])
@util.cross_origin(methods=["GET"])
@api_util.requires_login(accept_key=True)
def list_opponent_bots(user_id):
    bucket = model.get_gym_bot_bucket()
    files = [os.path.splitext(blob.name)[0] for blob in bucket.list_blobs()]
    files.sort()
    return flask.jsonify(files)
