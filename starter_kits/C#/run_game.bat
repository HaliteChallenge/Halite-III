mkdir replays
dotnet build -o ./build

REM Use below to run normally without debugger
halite.exe --replay-directory replays/ -vvv --width 32 --height 32 "dotnet ./build/Halite3.dll" "dotnet ./build/Halite3.dll"

REM Use below to stop timeouts if you want to attach a debugger
REM halite.exe --replay-directory replays/ -vvv --width 32 --height 32 --no-timeout "dotnet ./build/Halite3.dll" "dotnet ./build/Halite3.dll"
