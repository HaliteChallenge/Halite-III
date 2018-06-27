#!/usr/bin/env bash

set -e

cmake .
make MyBot
./halite --width 128 --height 128 "./MyBot" "./MyBot"
