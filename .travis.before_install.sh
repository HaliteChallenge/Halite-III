#!/bin/bash

set -x
set -e

env

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
    brew install lcov
else
    docker pull ubuntu:bionic
    docker run -itd --name build -v $(pwd):/project ubuntu:bionic /bin/bash
    docker exec build apt-get update
    docker exec build apt-get install -y cmake lcov $COMPILER
fi
