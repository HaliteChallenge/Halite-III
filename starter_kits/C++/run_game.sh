#!/usr/bin/env bash

set -e

cmake .
make
./halite --replay-directory replays/ -vvv --width 32 --height 32 "./MyBot" "./MyBot"
