#!/bin/sh

set -e

kotlinc MyBot.kt hlt/*.kt
./halite --replay-directory replays/ -vvv --width 32 --height 32 "java MyBot" "java MyBot"
