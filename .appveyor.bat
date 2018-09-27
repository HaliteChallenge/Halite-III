@echo on

cd C:\Halite\game_engine

setlocal
echo "Building game engine for AMD64."
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake -H. -Bbuild
cmake --build build --config Release
endlocal

setlocal
echo "Building game engine for x86."
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
cmake -H. -Bbuild32
cmake --build build32 --config Release
endlocal
