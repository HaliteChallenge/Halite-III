#!/bin/sh

set -e

swiftc Halite-III/Halite-III/main.swift Halite-III/Halite-III/hlt/*.swift -o MyBot
./halite --replay-directory replays/ -vvv --width 32 --height 32 "./MyBot" "./MyBot"
