import logging
import urllib.request
import requests
from hashlib import md5
import json
import os
from time import gmtime, strftime, sleep


# Needs to match corresponding value in apiserver/config.py
# This is the default value, 100 MiB
MAX_BOT_UPLOAD_SIZE = 100 * 1024 * 1024
# Maximum wait time in between compiled bot archive upload attempts,
# in seconds
MAX_UPLOAD_BACKOFF = 32


with open("config.json") as configfile:
    config = json.load(configfile)
    MANAGER_URL = config["MANAGER_URL"]
    SECRET_FOLDER = config["SECRET_FOLDER"]
    CAPABILITIES = config.get("CAPABILITIES", [])
    SERVER_ID = config.get("RANDOM_ID", "notset")
    provided_size = config.get("MAX_BOT_UPLOAD_SIZE", MAX_BOT_UPLOAD_SIZE)
    if provided_size:
        MAX_BOT_UPLOAD_SIZE = provided_size


def getTask(kind="task"):
    """Gets either a run or a compile task from the API"""
    params = {
        "capability": CAPABILITIES,
    }
    content = requests.get(MANAGER_URL + kind, params=params).text

    logging.info("Task call %s\n" % content)
    if content == "null":
        return None
    else:
        return json.loads(content)


def getBotHash(user_id, bot_id, is_compile=False):
    """Gets the checksum of a user's bot's zipped source code"""
    params = {
        "user_id": user_id,
        "bot_id": bot_id
    }
    if is_compile:
        params["compile"] = 1

    result = requests.get(MANAGER_URL+"botHash", params=params)

    logging.debug("Getting bot hash %s\n" % result.text)
    return json.loads(result.text).get("hash")


def storeBotLocally(user_id, bot_id, storage_dir, is_compile=False):
    """
    Download and store a bot's zip file locally

    Checks the file's checksum to make sure the file was downloaded properly
    """

    iterations = 0
    while iterations < 100:
        url = MANAGER_URL + "botFile?user_id={}&bot_id={}".format(user_id, bot_id)
        if is_compile:
            url += "&compile=1"

        logging.debug("Bot file url %s\n" % url)

        remote_zip = urllib.request.urlopen(url)
        zip_filename = remote_zip.headers.get('Content-disposition').split("filename")[1]
        zip_path = os.path.join(storage_dir, zip_filename)
        if os.path.exists(zip_path):
            os.remove(zip_path)

        remote_zip_contents = remote_zip.read()
        remote_zip.close()

        local_zip = open(zip_path, "wb")
        local_zip.write(remote_zip_contents)
        local_zip.close()

        content_hash = md5(remote_zip_contents).hexdigest()
        remote_hash = remote_zip.headers.get("X-Hash")
        if content_hash != remote_hash:
            iterations += 1
            continue

        return zip_path

    raise RuntimeError("Could not download bot with valid hash, aborting")


def storeBotRemotely(user_id, bot_id, zip_file_path):
    """Posts a bot file to the manager"""
    zip_contents = open(zip_file_path, "rb").read()
    if len(zip_contents) > MAX_BOT_UPLOAD_SIZE:
        raise RuntimeError("Bot archive exceeds maximum size of 100 MiB.")

    iterations = 0
    local_hash = md5(zip_contents).hexdigest()
    backoff = 1

    while iterations < 10:
        r = requests.post(MANAGER_URL+"botFile",
                          data={
                              "user_id": str(user_id),
                              "bot_id": str(bot_id),
                          },
                          files={"bot.zip": zip_contents})
        logging.debug("Posting compiled bot archive %s\n" % r.text)
        if r.status_code >= 400 and r.status_code <= 499:
            logging.error("Got a 4xx status code %s", r.status_code)
            r.raise_for_status()

        # Try again if local and remote hashes differ
        if local_hash != getBotHash(user_id, bot_id):
            logging.debug("Hashes do not match! Redoing file upload...\n")
            iterations += 1
            sleep(backoff)
            if backoff < MAX_UPLOAD_BACKOFF:
                backoff *= 2

            continue

        return

    raise RuntimeError("Could not upload bot with valid hash, aborting")


def compileResult(user_id, bot_id, did_compile, language, errors=None):
    """Posts the result of a compilation task"""
    r = requests.post(MANAGER_URL+"compile", data={
        "user_id": user_id,
        "bot_id": bot_id,
        "did_compile": int(did_compile),
        "language": language,
        "errors": errors,
        "server_id": SERVER_ID,
        "capability": CAPABILITIES,
    })
    logging.debug("Posted compile result %s\n" % r.text)


def gameResult(users, game_output, extra_metadata, url_path="game"):
    """
    POST the results of a game to the game coordinator.
    :param users:
    :param game_output: The parsed JSON result the game gives in quiet mode.
    :param challenge: A dictionary of extra metadata to pass back to
    the coordinator.
    :return:
    """

    replay_path = game_output["replay"]
    logging.debug("Posting game result %s (GMT)\n" % str(strftime("%Y-%m-%d %H:%M:%S", gmtime())))
    files = {os.path.basename(replay_path): open(replay_path, "rb").read()}
    for path in game_output["error_logs"].values():
        files[os.path.basename(path)] = open(path, "rb").read()

    for user in users:
        if user.get('bot_logs'):
            log_filename = '{}_{}_{}.log'.format(user['user_id'], user['bot_id'], user['username'])
            files[log_filename] = user['bot_logs']
            user['log_filename'] = log_filename

    data = {
        "users": json.dumps(users),
        "game_output": json.dumps(game_output),
        "server_id": SERVER_ID,
        "capability": CAPABILITIES,
    }
    for key, value in extra_metadata.items():
        data[key] = json.dumps(value)

    logging.info("Uploading game result")
    logging.info(json.dumps(users, indent=4))
    logging.info(json.dumps(game_output, indent=4))
    r = requests.post(MANAGER_URL + url_path, data=data, files=files)

    logging.info("Got game result %s (GMT)\n" % str(strftime("%Y-%m-%d %H:%M:%S", gmtime())))
    logging.debug("\n-------Game result:-----")
    logging.info(r.text)
    logging.debug("------------------------\n")


def ondemandResult(users, game_output, extra_metadata):
    gameResult(users, game_output, extra_metadata, url_path="ondemand_result")


def ondemandError(users, extra_metadata, language, log):
    """
    POST a compilation error that occurred during an ondemand game.
    """

    data = {
        "users": json.dumps(users),
        "language": language,
        "log": log,
    }
    for key, value in extra_metadata.items():
        data[key] = json.dumps(value)

    r = requests.post(MANAGER_URL + "ondemand_compile", data=data)
    logging.debug("\n-------Game result:-----")
    logging.debug(r.text)
    logging.debug("------------------------\n")
