dotnet build
dotnet restore
.\halite.exe --replay-directory replays/ -vvv --width 32 --height 32 "dotnet bin\Debug\netcoreapp2.0\MyBot.dll" "dotnet bin\Debug\netcoreapp2.0\MyBot.dll"
pause