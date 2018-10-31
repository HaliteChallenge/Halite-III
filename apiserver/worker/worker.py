import argparse
import copy
import os
import sys
import os.path
import glob
import json
import random
import shutil
import subprocess
import tempfile
import traceback
import logging
import uuid
import socket

from time import sleep, gmtime, strftime

import datetime
import threading
from flask import Flask

import archive
import backend
import compiler
import util

# Flask start
app = Flask(__name__)

# Log it real good
LOG_FILENAME = "worker-log-{}.data".format(uuid.uuid4())

# Constraints on # and size of log files read from bots
MAX_LOG_FILES = 1
MAX_LOG_FILE_SIZE = 50 * 1024  # 50 KiB

# Used to ensure system is running (watchdog timer)
TIME = datetime.datetime.now()
TIME_THRESHOLD = 60 * 18 # 18 mins in s

# Used by Watchdog timer to keep time
LOCK = threading.Lock()

# Where to create temporary directories
TEMP_DIR = os.getcwd()

# The game environment executable.
ENVIRONMENT = "halite"

# The script used to start the bot. This is either user-provided or
# created by compile.py.
RUNFILE = "run.sh"

# The command used to run the bot. On the outside is a cgroup limiting CPU
# and memory access. On the inside, we run the bot as a user so that it may
# not overwrite files. The worker image has a built-in iptables rule denying
# network access to this user as well.
BOT_COMMAND = "cgexec -g cpu,memory,devices,cpuset:{cgroup} sudo -Hiu {bot_user} bash -c 'cd \"{bot_dir}\" && ./{runfile}'"


COMPILE_ERROR_MESSAGE = """
Your bot caused unexpected behavior in our servers. If you cannot figure out
why this happened, please email us at halite@halite.io. We can help.

For our reference, here is the trace of the error:
"""


UPLOAD_ERROR_MESSAGE = """
We had some trouble uploading your bot. If you cannot figure out why
this happened, please email us at halite@halite.io. We can help.

For our reference, here is the trace of the error:
"""

class OndemandCompileError(Exception):
    """
    Error for when compilation fails before an ondemand game.
    """

    def __init__(self, language, log):
        self.language = language
        self.log = log


def makePath(path):
    """Deletes anything residing at path, creates path, and chmods the directory"""
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path)
    os.chmod(path, 0o777)


def give_ownership(top_dir, group, dir_perms):
    """Give ownership of everything in a directory to a given group."""
    for dirpath, _, filenames in os.walk(top_dir):
        shutil.chown(dirpath, group=group)
        os.chmod(dirpath, dir_perms)
        for filename in filenames:
            shutil.chown(os.path.join(dirpath, filename), group=group)
            os.chmod(os.path.join(dirpath, filename), dir_perms)


def rm_as_user(user, directory):
    """Remove a directory tree as the specified user."""
    subprocess.call(["sudo", "-H", "-u", user, "-s", "rm", "-rf", directory],
                    stderr=subprocess.DEVNULL,
                    stdout=subprocess.DEVNULL)


def executeCompileTask(user_id, bot_id, backend):
    """Downloads and compiles a bot. Posts the compiled bot files to the manager."""
    logging.debug("Compiling a bot with userID %s\n" % str(user_id))

    errors = []

    with tempfile.TemporaryDirectory(dir=TEMP_DIR) as temp_dir:
        try:
            bot_path = backend.storeBotLocally(user_id, bot_id, temp_dir,
                                               is_compile=True)
            archive.unpack(bot_path)

            # Make sure things are in the top-level directory
            while len([
                name for name in os.listdir(temp_dir)
                if os.path.isfile(os.path.join(temp_dir, name))
            ]) == 0 and len(glob.glob(os.path.join(temp_dir, "*"))) == 1:
                with tempfile.TemporaryDirectory(dir=TEMP_DIR) as bufferFolder:
                    singleFolder = glob.glob(os.path.join(temp_dir, "*"))[0]

                    for filename in os.listdir(singleFolder):
                        shutil.move(os.path.join(singleFolder, filename), bufferFolder)
                    os.rmdir(singleFolder)

                    for filename in os.listdir(bufferFolder):
                        shutil.move(os.path.join(bufferFolder, filename), temp_dir)
                    # Context manager takes care of buffer folder

            # Delete any symlinks
            subprocess.call(["find", temp_dir, "-type", "l", "-delete"])

            # Give the compilation user access
            os.chmod(temp_dir, 0o755)
            # User needs to be able to write to the directory and create files
            give_ownership(temp_dir, "bots", 0o2770)

            # Reset cwd before compilation, in case it was in a
            # deleted temporary folder
            os.chdir(os.path.dirname(os.path.realpath(sys.argv[0])))
            language, more_errors = compiler.compile_anything(temp_dir)
            didCompile = more_errors is None
            if more_errors:
                errors.extend(more_errors)
        except Exception:
            language = "Other"
            errors = [COMPILE_ERROR_MESSAGE + traceback.format_exc()] + errors
            didCompile = False

        try:
            if didCompile:
                logging.debug("Bot did compile\n")

                # Make things group-readable
                subprocess.call([
                    "sudo", "-H", "-u", "bot_compilation", "-s",
                    "chmod", "-R", "g+r", temp_dir,
                ], stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL)
                archive_path = os.path.join(temp_dir, str(user_id)+".zip")
                archive.zipFolder(temp_dir, archive_path)
                backend.storeBotRemotely(user_id, bot_id, archive_path)
            else:
                logging.debug("Bot did not compile\n")
                logging.debug("Bot errors %s\n" % str(errors))


            backend.compileResult(user_id, bot_id, didCompile, language,
                                  errors=(None if didCompile else "\n".join(errors)))
        except Exception as e:
            logging.error("Bot did not upload", e)
            errors.append(UPLOAD_ERROR_MESSAGE + traceback.format_exc())
            backend.compileResult(user_id, bot_id, False, language,
                                  errors="\n".join(errors))
        finally:
            # Remove files as bot user (Python will clean up tempdir, but we don't
            # necessarily have permissions to clean up files)
            rm_as_user("bot_compilation", temp_dir)


def setupParticipant(user_index, user, temp_dir):
    """
    Download and set up the bot for a game participant.
    """
    # Include username to deal with duplicate bots
    bot_dir = "{}_{}_{}".format(user["user_id"], user["bot_id"], user["username"])
    bot_dir = os.path.join(temp_dir, bot_dir)
    os.mkdir(bot_dir)
    archive.unpack(backend.storeBotLocally(user["user_id"],
                                           user["bot_id"], bot_dir))

    if user.get("requires_compilation"):
        compile_dir = bot_dir + '_compile'
        try:
            # Move to temp directory to avoid permission problems
            # (can't chown files created by compile user back to us)
            shutil.move(bot_dir, compile_dir)

            # Give the compilation user access
            os.chmod(compile_dir, 0o2755)
            # User needs to be able to write to the directory
            give_ownership(compile_dir, "bots", 0o2774)

            language, errors = compiler.compile_anything(compile_dir)
            didCompile = errors is None
        except Exception:
            language = "Other"
            errors = [COMPILE_ERROR_MESSAGE + traceback.format_exc()] + errors
            didCompile = False

        if not didCompile:
            # Abort and upload an error log
            rm_as_user("bot_compilation", compile_dir)
            raise OndemandCompileError(language, '\n'.join(errors))

        # Move back to original directory
        try:
            shutil.copytree(compile_dir, bot_dir)
        except shutil.Error as e:
            logging.error("Could not compile bot ondemand", e)

        rm_as_user("bot_compilation", compile_dir)

    # Make the start script executable
    os.chmod(os.path.join(bot_dir, RUNFILE), 0o755)

    # Give the bot user ownership of their directory
    # We should set up each user's default group as a group that the
    # worker is also a part of. Then we always have access to their
    # files, but not vice versa.
    # https://superuser.com/questions/102253/how-to-make-files-created-in-a-directory-owned-by-directory-group

    bot_user = "bot_{}".format(user_index)
    bot_group = "bots_{}".format(user_index)
    bot_cgroup = "bot_{}".format(user_index)

    # We want 770 so that the bot can create files still; leading 2
    # is equivalent to g+s which forces new files to be owned by the
    # group
    give_ownership(bot_dir, bot_group, 0o2770)

    bot_command = BOT_COMMAND.format(
        cgroup=bot_cgroup,
        bot_dir=bot_dir,
        bot_group=bot_group,
        bot_user=bot_user,
        runfile=RUNFILE,
    )
    bot_name = "{} v{}".format(user["username"], user["version_number"])

    return bot_command, bot_name, bot_dir


def runGame(environment_parameters, users, offset=0):
    with tempfile.TemporaryDirectory(dir=TEMP_DIR) as temp_dir:
        shutil.copy(ENVIRONMENT, os.path.join(temp_dir, ENVIRONMENT))

        command = [
            "./" + ENVIRONMENT,
            "--results-as-json",
        ]

        for key, value in environment_parameters.items():
            command.append("--{}".format(key))
            if value:
                command.append("{}".format(value))

        # Make sure bots have access to the temp dir as a whole
        # Otherwise, Python can't import modules from the bot dir
        # Based on strace, Python lstat()s the full dir path to the dir it's
        # in, and fails when it tries to lstat the temp dir, which this
        # fixes
        os.chmod(temp_dir, 0o755)

        for user_index, user in enumerate(users):
            bot_command, bot_name, bot_dir = setupParticipant(user_index + offset, user, temp_dir)
            command.append(bot_command)
            command.append("-o")
            command.append(bot_name)
            user['bot_dir'] = bot_dir

        logging.debug("Run game command %s\n" % command)
        logging.debug(command)
        logging.debug("Waiting for game output...\n")
        lines = subprocess.Popen(
            command,
            stdout=subprocess.PIPE).stdout.read().decode('utf-8').split('\n')
        logging.debug("\n-----Here is game output: -----")
        logging.debug("\n".join(lines))
        logging.debug("--------------------------------\n")

        # tempdir will automatically be cleaned up, but we need to do things
        # manually because the bot might have made files it owns
        for user_index, user in enumerate(users):
            # keep any bot logs
            user['bot_logs'] = ''
            log_files_read = 0
            for filename in os.listdir(user['bot_dir']):
                try:
                    _, ext = os.path.splitext(filename)
                    if ext.lower() == '.log':
                        log_files_read += 1
                        user['bot_logs'] += '===== Log file {}\n'.format(filename)
                        with open(os.path.join(user['bot_dir'], filename)) as logfile:
                            user['bot_logs'] += logfile.read(MAX_LOG_FILE_SIZE)
                        user['bot_logs'] += '\n===== End of log {}\n'.format(filename)
                except Exception:
                    # Ignore log and move on if we fail
                    pass

                if log_files_read >= MAX_LOG_FILES:
                    break

            bot_user = "bot_{}".format(user_index + offset)
            rm_as_user(bot_user, temp_dir)

            # The processes won't necessarily be automatically cleaned up, so
            # let's do it ourselves
            util.kill_processes_as(bot_user)

        return lines


def parseGameOutput(output, users):
    users = copy.deepcopy(users)

    logging.debug(output)
    result = json.loads(output)

    for player_tag, stats in result["stats"].items():
        player_tag = int(player_tag)
        users[player_tag]["player_tag"] = player_tag
        users[player_tag]["rank"] = stats["rank"]
        users[player_tag]["timed_out"] = False
        users[player_tag]["log_name"] = None

    for player_tag, error_log in result["error_logs"].items():
        numeric_player_tag = int(player_tag)
        users[numeric_player_tag]["timed_out"] = result["terminated"].get(player_tag, False)
        users[numeric_player_tag]["log_name"] = os.path.basename(error_log)

    return users, result


def executeGameTask(environment_parameters, users, extra_metadata, gameResult):
    """Downloads compiled bots, runs a game, and posts the results of the game"""
    logging.debug("Running game with parameters {}\n".format(environment_parameters))
    logging.debug("Users objects {}\n".format(users))
    logging.debug("Extra metadata {}\n".format(extra_metadata))

    raw_output = '\n'.join(runGame(
        environment_parameters, users,
        extra_metadata.get("offset", 0)))
    users, parsed_output = parseGameOutput(raw_output, users)

    gameResult(users, parsed_output, extra_metadata)

    # Clean up game logs and replays
    filelist = glob.glob("*.log")
    for f in filelist:
        os.remove(f)
    os.remove(parsed_output["replay"])

    # Make sure game processes exit
    subprocess.run(["pkill", "--signal", "9", "-f", "cgexec"])


def _set_logging():
    logging.basicConfig(filename=LOG_FILENAME, level=logging.INFO)
    logging.getLogger('werkzeug').setLevel(logging.ERROR)
    logging.getLogger('requests').setLevel(logging.CRITICAL)
    outLog = logging.StreamHandler(sys.stdout)
    outLog.setLevel(logging.DEBUG)
    outLog.setFormatter(logging.Formatter('%(asctime)s [%(levelname)s]: %(message)s'))
    logging.getLogger().addHandler(outLog)

def set_time():
    global LOCK
    with LOCK:
        global TIME
        TIME = datetime.datetime.now()
        logging.info("Setting time to {}".format(TIME))

def is_time_up_to_date():
    global LOCK
    with LOCK:
        global TIME
        current_time = datetime.datetime.now()
        logging.info("TIME DIFFERENCE: {}".format((current_time - TIME).total_seconds()))
        if (current_time - TIME).total_seconds() > TIME_THRESHOLD:
            return False
        return True

@app.route('/health_check')
def health_check():
    if is_time_up_to_date():
        return "Alive", 200
    else:
        return "Dead. Last alive at {}".format(TIME), 503

def main(args):
    _set_logging()
    logging.info("Starting up worker at {}".format(socket.gethostname()))
    threading.Thread(target=app.run, kwargs={'host':'0.0.0.0', 'port':5001, 'threaded':True}).start()
    while True:
        set_time()
        try:
            logging.debug("\n\n\nQuerying for new task at time %s (GMT)\n" % str(strftime("%Y-%m-%d %H:%M:%S", gmtime())))

            task = backend.getTask(args.task_type)
            if "type" in task and (task["type"] == "compile" or task["type"] == "game"):
                logging.debug("Got new task at time %s (GMT)\n" % str(strftime("%Y-%m-%d %H:%M:%S", gmtime())))
                logging.debug("Task object %s\n" % str(task))

                if task["type"] == "compile":
                    logging.debug("Running a compilation task...\n")
                    executeCompileTask(task["user"], task["bot"], backend)
                else:
                    logging.debug("Running a game task...\n")
                    executeGameTask(task.get("environment_parameters", {}),
                                    task["users"], {
                                        "challenge": task.get("challenge"),
                                    }, backend.gameResult)
            elif task.get("type") == "ondemand":
                environment_params = task["environment_parameters"]
                extra_metadata = {
                    "task_user_id": task["task_user_id"],
                    "offset": int(args.user_offset),
                }

                try:
                    executeGameTask(environment_params,
                                    task["users"],
                                    extra_metadata,
                                    backend.ondemandResult)
                except OndemandCompileError as e:
                    backend.ondemandError(
                        task["users"],
                        extra_metadata,
                        e.language, e.log
                    )
            else:
                logging.debug("No task available at time %s (GMT). Sleeping...\n" % str(strftime("%Y-%m-%d %H:%M:%S", gmtime())))
                sleep(random.randint(1, 4))
        except Exception as e:
            logging.exception("Error on get task %s\n" % str(e))

            logging.debug("Sleeping...\n")
            sleep(random.randint(1, 4))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--task-type", default="task")
    parser.add_argument("--user-offset", default=0)
    args = parser.parse_args()
    main(args)
