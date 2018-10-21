#!/bin/sh

set -e

./halite --replay-directory replays/ -vvv --width 32 --height 32 "php MyBot.php" "php MyBot.php"
