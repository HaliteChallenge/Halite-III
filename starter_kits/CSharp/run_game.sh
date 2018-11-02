#!/bin/sh
dotnet build
./halite --replay-directory replays/ -vvv --width 32 --height 32 "dotnet Halite3/bin/Debug/netcoreapp2.0/MyBot.dll" "dotnet Halite3/bin/Debug/netcoreapp2.0/MyBot.dll"
