#!/bin/sh

set -e

sbt compile
./halite --replay-directory replays/ -vvv --width 32 --height 32 "java MyScalaBot" "java MyScalaBot"
