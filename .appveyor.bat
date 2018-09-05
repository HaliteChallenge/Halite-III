cd C:\Halite\game_engine

setlocal
call "%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
.\make.bat
endlocal

setlocal
call "%ProgramFiles(x86)%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
cmake -H. -Bbuild32
cmake --build build32 --config Release
endlocal
