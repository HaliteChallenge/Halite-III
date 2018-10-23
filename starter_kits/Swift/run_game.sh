#!/bin/sh

set -e

swiftc MyBot.swift
./halite --replay-directory replays/ -vvv --width 32 --height 32 "./MyBot" "./MyBot"
