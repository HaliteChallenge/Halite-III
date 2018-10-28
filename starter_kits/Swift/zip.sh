#!/bin/sh

set -e
cp -r Halite-III/Halite-III Files
touch Files/MyBot.swift
zip -9 -r --exclude=*DS_Store* MyBot.zip Files/
rm -r Files/
