#!/bin/sh

set -e
cp -r Halite-III FilesToZip
touch FilesToZip/MyBot.swift
zip -9 -r --exclude=*DS_Store* --exclude=*.build* --exclude=*.xcodeproj* --exclude=*Halite-IIITests* MyBot.zip FilesToZip
rm -r FilesToZip/
