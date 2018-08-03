#!/usr/bin/env bash

set -e

make CXXFLAGS='-O2 -Wall -Werror -Wno-unused-function -Wno-unused-variable -pedantic -std=c++14' MyBot
./halite --width 32 --height 32 "./MyBot" "./MyBot"
