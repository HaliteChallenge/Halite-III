#!/bin/sh

set -e

swiftc main.swift hlt/*.swift -o MyBot
./halite --replay-directory replays/ -vvv --width 32 --height 32 "./MyBot" "./MyBot"
