#!/usr/bin/env python3

import sys

name = sys.argv[1]
commands = sys.argv[2].split(',')

# Parse input properly
input()  # Constants
num_players, _ = map(int, input().split())
for _ in range(num_players):
    input()
_, map_height = map(int, input().split())
for _ in range(map_height):
    input()

print(name)

index = 0

while True:
    input()  # Turn number
    for _ in range(num_players):
        _, num_ships, num_dropoffs, _ = map(int, input().split())
        for _ in range(num_ships + num_dropoffs):
            input()
    for _ in range(int(input())):
        input()
    if 0 <= index < len(commands):
        print(commands[index])
        index += 1
    else:
        print()
