#!/usr/bin/env bash

set -e

cmake .
make
./halite -vvv --width 32 --height 32 "./MyBot" "./MyBot"
