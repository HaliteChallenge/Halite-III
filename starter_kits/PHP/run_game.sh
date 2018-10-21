#!/bin/sh

set -e

./halite --replay-directory replays/ -vvv --width 32 --height 32 "php app.php" "php app.php"
