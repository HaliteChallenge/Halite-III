#!/bin/sh

set -e
cp -r Halite-III FilesToZip
zip -9 -r --exclude=*DS_Store* --exclude=*.build* --exclude=*.xcodeproj* --exclude=*Halite-IIITests* MyBot.zip FilesToZip
rm -r FilesToZip/
