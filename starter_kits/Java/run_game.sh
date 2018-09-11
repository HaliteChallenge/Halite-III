#!/bin/sh

set -e

javac MyBot.java hlt/*.java
./halite -vvv --width 32 --height 32 "java MyBot" "java MyBot"
