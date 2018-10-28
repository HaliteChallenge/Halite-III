#!/usr/bin/env bash

set -e

make
./halite --replay-directory replays/ -vvv --width 32 --height 32 "./MyBot" "./MyBot"
