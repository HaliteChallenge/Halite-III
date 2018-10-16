#!/bin/sh

set -e

dotnet build -o ./build
./halite --replay-directory replays/ -vvv --width 32 --height 32 "dotnet ./build/Halite3.dll" "dotnet ./build/Halite3.dll"
