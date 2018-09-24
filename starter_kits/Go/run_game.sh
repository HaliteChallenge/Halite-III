#!/bin/sh

set -e
go build main

./halite --replay-directory replays/ -vvv --width 32 --height 32 "go run main" "go run main"
