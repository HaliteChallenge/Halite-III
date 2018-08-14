import io
import os
import sys
import pathlib
import zipfile
import requests
from . import client, output, util

_BOT_FILE_NAME_PREPEND = 'MyBot.'
_LANGUGAGE_PROJECT_FILE_IDENTIFIERS = ['cargo.toml', 'project.clj', 'package.swift', 'stack.yaml']
_HALITE_LIBRARY_FOLDER = 'hlt/'
_FIRST_BOT_INDEX = 0
_VERSION_NUMBER_KEY = 'version_number'

# File extensions included in a bot upload by default
_BOT_FILE_EXTENSION_WHITELIST = {'.py', '.java', '.cpp', '.cxx', '.c', '.h', '.hpp'}


def _bot_exists(user_id):
    """
    Whether a bot has been previously uploaded. Assumes that a user only has bot 0
    :param user_id: The id of the user making the request
    :return: True if created, False otherwise
    """
    return requests.get(client.URI_API_EXISTING_BOT.format(user_id, client.FIRST_BOT_ID)).status_code == client.SUCCESS


def _get_bot_version(user_id):
    """
    Determined the version of the latest bot for a user
    :param user_id: The id of the user making the request
    :return: The bot id
    """
    return requests.get(client.URI_API_CREATE_BOT.format(user_id)).json()[_FIRST_BOT_INDEX][_VERSION_NUMBER_KEY]


def _upload_bot(user_id, api_key, bot_file):
    """
    Uploads the bot to the Halite servers. If the bot exists, simply update using PUT. Otherwise create it with
    POST. NOTE: Assuming only bot is bot 0
    :param user_id: The ID of the user making the request
    :param api_key: The API key provided by the server (acquired in the auth phase)
    :param bot_path: The path wherein the bot is found (expected file)
    :return: The result of the request to the server
    """
    files = {client.BOT_FILE_KEY: bot_file}
    headers = {client.API_KEY_HEADER: api_key}
    if _bot_exists(user_id):
        return requests.put(client.URI_API_EXISTING_BOT.format(user_id, client.FIRST_BOT_ID), files=files, headers=headers)
    else:
        return requests.post(client.URI_API_CREATE_BOT.format(user_id), files=files, headers=headers)


def _download_bot(user_id, api_key, bot_path):
    """
    Download the bot from the Halite servers.

    NOTE: Assuming only bot is bot 0

    :param user_id: The ID of the user making the request
    :param api_key: The API key provided by the server (acquired in the auth phase)
    :param bot_path: The path wherein the bot is found (expected file)
    :return: The result of the request to the server
    """
    headers = {
        client.API_KEY_HEADER: api_key,
        "Accept": "application/zip"
    }
    if _bot_exists(user_id):
        request = requests.get(client.URI_API_EXISTING_BOT.format(user_id, client.FIRST_BOT_ID), headers=headers)
        with open(bot_path, "wb") as target:
            target.write(request.content)
        return request
    else:
        raise ValueError("Bot does not exist.")


def _zip_file_integrity_check(file_path):
    """
    Determines whether the zip file contents are structured correctly to be uploaded.
    That includes the zip being properly zipped and the correct files being placed
    :param file_path:
    :return:
    """
    try:
        zip = zipfile.ZipFile(file_path)
    except zipfile.BadZipFile:
        raise TypeError("The file provided is not a proper zip file")
    except FileNotFoundError:
        raise FileNotFoundError("Could not find the zip file provided")
    if not any((item.startswith(_BOT_FILE_NAME_PREPEND)
                or item.lower() in _LANGUGAGE_PROJECT_FILE_IDENTIFIERS) for item in zip.namelist()):
        raise ValueError("MyBot.* file must be present in the zip's top directory (or cargo.toml in case of Rust).")
    if not any(item.lower().startswith(_HALITE_LIBRARY_FOLDER) for item in zip.namelist()):
        output.warning("WARNING: Could not find an hlt/ library folder. Proceeding with upload. {}".format(os.linesep))


def _create_bot_upload(bot_path, dry_run=False, extension_whitelist=None):
    """
    Given the path to a MyBot.* file, create a ZIP to upload.
    """

    if not extension_whitelist:
        extension_whitelist = _BOT_FILE_EXTENSION_WHITELIST

    files_to_include = []
    for dirpath, _, filenames in os.walk(os.path.dirname(bot_path)):
        for filename in filenames:
            _, ext = os.path.splitext(filename)
            if ext in extension_whitelist:
                files_to_include.append(os.path.join(dirpath, filename))
    output.print_list("Files that will be included:", files_to_include)
    if dry_run:
        output.output("Dry run, not continuing.")
        return

    # Create ZIP and upload
    bot_file = io.BytesIO()
    with zipfile.ZipFile(bot_file, mode='w') as bot_zip:
        for filename in files_to_include:
            destname = os.path.relpath(filename, os.path.dirname(bot_path))
            bot_zip.write(filename, arcname=destname)

    bot_file.seek(0)
    return bot_file


def upload(bot_path, dry_run, include_extensions):
    """
    Uploads the bot placed under bot_path. May only be called once Config is properly initialized.
    :param bot_path: The path wherein the bot is located
    :return: Nothing
    """

    bot_file = None

    # If the bot looks like a MyBot.* file, then try and create the
    # archive for the user.
    # We only support this for Python bots (for now), though.
    bot_filename = os.path.basename(bot_path)
    if os.path.exists(bot_path) and bot_filename.startswith(_BOT_FILE_NAME_PREPEND):
        bot_file = _create_bot_upload(bot_path, dry_run, include_extensions)
        if not bot_file:
            # Dry run
            return
    else:
        _zip_file_integrity_check(bot_path)
        if not bot_path or not os.path.isfile(bot_path):
            raise ValueError("Bot path is not valid or does not exist. Try again.")

    if dry_run:
        output.output("Dry run, not continuing.")
        return

    config = client.Config()
    with (bot_file if bot_file else open(bot_path, 'rb')) as bot_file:
        output.output("Uploading bot...")
        result = _upload_bot(config.user_id, config.api_key, bot_file)
        if result.status_code != client.SUCCESS:
            raise IOError("Unable to upload bot: {}".format(result.text))
        output.output("Successfully uploaded bot with version {}".format(_get_bot_version(config.user_id)))


def download(bot_path):
    """
    Downloads the bot to the file bot_path. May only be called once Config is properly initialized.
    :param bot_path: The path the bot should be written
    :return: Nothing
    """
    config = client.Config()
    bot_path = pathlib.Path(bot_path)

    if bot_path.exists():
        # Confirm overwriting
        if not util.confirm("{} already exists. Overwrite?".format(bot_path)):
            output.output("Aborting download.")
            return

    # Make the directories
    bot_path.parent.mkdir(parents=True, exist_ok=True)

    output.output("Downloading bot...")
    result = _download_bot(config.user_id, config.api_key, bot_path)
    if result.status_code != client.SUCCESS:
        raise IOError("Unable to download bot: {}".format(result.text))
    output.output("Successfully downloaded bot with version {}".format(_get_bot_version(config.user_id)))
