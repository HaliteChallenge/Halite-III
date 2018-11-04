#!/bin/bash

EMSCRIPTEN_ROOT=~/Code/emsdk/emscripten/1.38.15/

cmake \
    -Bembuild -H. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE=$EMSCRIPTEN_ROOT/cmake/Modules/Platform/Emscripten.cmake

cmake --build embuild -- -j4
