#!/bin/sh

set -e
go build main

./halite -vvv --width 32 --height 32 "go run main" "go run main"