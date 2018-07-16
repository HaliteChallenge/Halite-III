#!/usr/bin/env python3

import hlt
import random # Needed for choosing a random direction

game_map, players, myID = hlt.get_init() # Get the initial game state
hlt.send_init("MyPythonBot-"+str(myID)) # Respond with our name

while True:
	turn_number, players = hlt.get_frame() # Get the newest frame
	for loc in players[myID].ships.keys(): # For each of our ships
		# If we're on our shipyard and have enough halite, dump it.
		if loc == players[myID].shipyard and players[myID].ships[loc] > hlt.MAX_HALITE/4:
			hlt.dump(loc, players[myID].ships[loc])
		# Otherwise, check if there's a reasonable amount of halite on the square.
        # If so, extract halite. If not, move randomly.
		elif game_map[loc[1]][loc[0]] > hlt.MAX_HALITE / 10:
			pass # Do nothing, which is to say, extract halite.
		else:
			hlt.move(loc, random.choice(["n", "e", "s", "w"])) # Move in a random direction
	# If we're in the first 200 turns and have enough halite, spawn a ship.
	if turn_number <= 200 and players[myID].halite >= hlt.SHIP_COST:
		hlt.spawn(0) # We don't want to put any Halite in the newly spawned ship.
	hlt.send_frame() # Send our moves back to the game environment
