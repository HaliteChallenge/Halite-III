#!/bin/bash

set -x
set -e

env

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
    cd environment
    cmake .
    make
else
    docker exec build /bin/bash -c "which $CCOMPILE"
    docker exec build /bin/bash -c "which $CXXCOMPILE"
    docker exec build /bin/bash -c "$CXXCOMPILE --version"
    docker exec build /bin/bash -c "cd /project/environment; CC=$CCOMPILE CXX=$CXXCOMPILE cmake ."
    docker exec build /bin/bash -c "cd /project/environment; CC=$CCOMPILE CXX=$CXXCOMPILE make"
fi
