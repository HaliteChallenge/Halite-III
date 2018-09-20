#!/bin/sh

set -e

sbt compile
./halite -vvv --width 32 --height 32 "java MyScalaBot" "java MyScalaBot"
