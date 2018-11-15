#!/bin/sh

set -e

jar="target/MyBot.jar"
if [ ! -e "$jar" ]; then
    lein uberjar
fi
seed=""
bot0="java -jar $jar $seed"
bot1=$bot0

./halite --replay-directory replays/ -vvv --width 32 --height 32 "$bot0" "$bot1"
