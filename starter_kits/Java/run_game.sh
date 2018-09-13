#!/bin/sh

set -e

javac MyBot.java
./halite -vvv --width 32 --height 32 "java MyBot" "java MyBot"
