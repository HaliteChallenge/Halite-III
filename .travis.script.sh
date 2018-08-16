#!/bin/bash

set -x
set -e

env

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
    pushd environment
    cmake .
    make -j2
    popd

    pushd tools/halite-in-a-box
    pushd macos-portable-python
    ./build.sh
    popd

    cp -r macos-portable-python/dist/python-macos.app extra/python-macos
    rsync -r \
          --exclude __pycache__ --exclude '*~' --exclude '*.pyc' \
          ../hlt_client/hlt_client/ extra/hlt_client
    $(npm root)/.bin/electron-builder -m
else
    docker exec build /bin/bash -c "which $CCOMPILE"
    docker exec build /bin/bash -c "which $CXXCOMPILE"
    docker exec build /bin/bash -c "$CXXCOMPILE --version"
    docker exec build /bin/bash -c "cd /project/environment; CC=$CCOMPILE CXX=$CXXCOMPILE cmake ."
    docker exec build /bin/bash -c "cd /project/environment; CC=$CCOMPILE CXX=$CXXCOMPILE make -j2"
fi
