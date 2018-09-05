@echo on

cd C:\Halite\game_engine

setlocal
echo "Building game engine for AMD64."
call "%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
cmake -H. -Bbuild
cmake --build build --config Release
endlocal

setlocal
echo "Building game engine for x86."
call "%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
cmake -H. -Bbuild32
cmake --build build32 --config Release
endlocal
