#!/bin/bash

rm -r *.log
rm -r *.hlt
rm -r *.txt

# start the dotnet build
dotnet build

if [ $? -eq 0 ]
then
# run bots like it would do in our servers
./halite --replay-directory replays/ -vvv --width 32 --height 32 "MyBot.exe" "MyBot.exe"

# run bots without timeouts (good for debugging)
#./halite --replay-directory replays/ -vvv --width 32 --height 32 --no-timeouts "MyBot.exe" "MyBot.exe"
else
  echo "Build failed"
fi

