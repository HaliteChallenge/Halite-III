#!/bin/sh

./halite --replay-directory replays/ -vvv --width 32 --height 32 "python3 SVMBotAggressive.py" "python3 SVMBotPassive.py"
