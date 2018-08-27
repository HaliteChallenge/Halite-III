#!/bin/sh

javac MyBot.java hlt/*.java hlt/command/*.java hlt/util/*.java
./halite --width 32 --height 32 "java MyBot" "java MyBot"
