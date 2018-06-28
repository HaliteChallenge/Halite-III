#!/usr/bin/env bash

set -e

make MyBot
./halite --width 128 --height 128 "./MyBot" "./MyBot"
