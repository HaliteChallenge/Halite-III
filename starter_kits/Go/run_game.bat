go build main

halite.exe --replay-directory replays/ -vvv --width 32 --height 32 "go run main" "go run main"
