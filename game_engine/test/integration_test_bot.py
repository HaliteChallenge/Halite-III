#!/usr/bin/env python3

import re
import sys

name = sys.argv[1]
commands = sys.argv[2].split(',')

# Parse input properly
input()  # Constants
num_players, my_id = map(int, input().split())
for _ in range(num_players):
    input()
_, map_height = map(int, input().split())
for _ in range(map_height):
    input()

print(name)

index = 0

# Assign a monotonic index to ships we get, so that the driver script
# can refer to ships without having to assume anything about the IDs
# that the engine gives us.
ship_counter = 0
ship_map = {}
ship_re = re.compile(r'#(\d)+')

while True:
    input()  # Turn number
    for player_id in range(num_players):
        _, num_ships, num_dropoffs, _ = map(int, input().split())
        for _ in range(num_ships):
            ship_id, *_ = map(int, input().split())
            if player_id != my_id:
                continue
            if ship_id not in ship_map:
                ship_map[ship_counter] = ship_id
                ship_counter += 1
        for _ in range(num_dropoffs):
            input()
    for _ in range(int(input())):
        input()
    if 0 <= index < len(commands):
        print(ship_re.sub(lambda match: str(ship_map[int(match.group(1))]), commands[index]))
        index += 1
    else:
        print()
