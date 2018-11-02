# Halite Client

## Installation
Download the tool from our [downloads page](https://halite.io/learn-programming-challenge/downloads). Run `python setup.py install` to install the client.

* If you have multiple Python installations, make sure to install using Python 3. You may also need to invoke `hlt` commands with `python3 -m hlt_client` if `hlt` throws errors.
* You may have to `sudo` or use an admin shell.

## Usage
The Halite Client lets you interface with our game servers and play games locally using the command line.

**Authorize your client**

Before you can use most features of the client, you'll need to connect it to your Halite account. Have your Halite API key ready, or generate a new one at https://halite.io/user/settings.

`hlt auth`


**Upload a bot**

Uploads the specified bot onto the Halite game servers.

`hlt bot -b [PATH_TO_ZIP] upload`


**Download current bot**

Downloads your currently-playing bot into a zip archive.

`hlt bot -b [NAME].zip download`


**Run any number of games**

This command runs the number of games you specify, creating save logs and replays as normal, and tells you the win ratio as games are played.

`hlt play -r [BOT_COMMAND] -r [BOT_COMMAND] -b [PATH_TO_HALITE.EXE] --output-dir [OUTPUT_LOCATION] -i [NUMBER_OF_ITERATIONS]`

* You may give two or four `-r [BOT_COMMAND]` parameters to run two or four player games.
* `[BOT_COMMAND]` allows you to specify how to run a bot. The command defaults to Python 3; if only a path is given, the client will run the bot assuming it is a Python bot. You may also mix and match: `hlt play -r "python3 Halite3_Py/MyBot.py" -r "ruby Halite3_Ruby/MyBot.rb" ...` allows you to compare bots in different languages.
* The halite.exe game binary is required to play games. Specify the path after the -b (--binary) parameter: `-b Halite3_Py/halite`
* If you'd like to specify the game map size, you can add `-W [WIDTH] -H [HEIGHT]` parameters. Game maps are 32x32, 40x40, 48x48, 56x56, or 64x64 in width and height.


**Download replays**

  * By user: `hlt replay user -i [USER_ID] -l [NUMBER_OF_FILES_WANTED] -d [DOWNLOAD_LOCATION]`
    This command will download .hlt replay files of a user by id.

  * By date: `hlt replay date -t [DATE] -d [DOWNLOAD_LOCATION]`

    This command will download .hlt replay files of all gold ranked games on the given date. The date format is eight numbers YYYYMMDD.

    Adding the `-a` flag will include silver replays as well.

Adding the `--decompress` flag to the end of any replay download command will download .json instead of .hlt replay files.
