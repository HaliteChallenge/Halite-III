#!/bin/sh

set -e
cp -r Halite-III/Halite-III Files
zip -9 -r --exclude=*DS_Store* MyBot.zip Files/
rm -r Files/
