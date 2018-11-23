#!/bin/sh

npm run tslint && npm run tsc && ./halite --replay-directory replays/ -vvv --width 32 --height 32 "node MyBot.js" "node MyBot.js"
