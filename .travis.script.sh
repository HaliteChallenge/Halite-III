#!/bin/bash

set -x
set -e

env

mkdir -p artifacts/
if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
    pushd game_engine
    cmake -DCMAKE_BUILD_TYPE=Release .
    make -j2
    cp ./halite ../artifacts/Halite-MacOS-$(date +%Y%m%d%H%M%S)-$(git rev-parse HEAD)
    popd

    cd ../
    ls artifacts
else
    docker exec build /bin/bash -c "which $CCOMPILE"
    docker exec build /bin/bash -c "which $CXXCOMPILE"
    docker exec build /bin/bash -c "$CXXCOMPILE --version"
    docker exec build /bin/bash -c "cd /project/game_engine; CC=$CCOMPILE CXX=$CXXCOMPILE cmake -DCMAKE_BUILD_TYPE=Release ."
    docker exec build /bin/bash -c "cd /project/game_engine; CC=$CCOMPILE CXX=$CXXCOMPILE make -j2"
    cp ./game_engine/halite ./artifacts/Halite-Linux-$(date +%Y%m%d%H%M%S)-$(git rev-parse HEAD)
fi
