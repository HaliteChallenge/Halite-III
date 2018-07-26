#!/usr/bin/env python3

import hlt
import random # Needed for choosing a random direction

game = hlt.Bot()
game_map, players, myID = game.get_init() # Get the initial game state
game.send_init("MyPythonBot-" + str(myID)) # Respond with our name

while True:
    turn_number, game_map, players, commands = game.get_frame() # Get the newest frame

    at_home = False
    for ship in players[myID].ships.values(): # For each of our ships
        # If we're on our shipyard and have enough halite, dump it.
        if ship.location == players[myID].shipyard and ship.halite > hlt.MAX_HALITE / 4:
            commands.dump(ship.id, ship.halite)
            at_home = True
            # Otherwise, check if there's a reasonable amount of halite on the square.
            # If so, extract halite. If not, move randomly.
        elif game_map[ship.location[1]][ship.location[0]] > hlt.MAX_HALITE / 10:
            pass # Do nothing, which is to say, extract halite.
        else:
            commands.move(ship.id, random.choice(["n", "e", "s", "w"])) # Move in a random direction
            # If we're in the first 200 turns and have enough halite, spawn a ship.
    if turn_number <= 200 and players[myID].halite >= hlt.SHIP_COST:
        if len(players[myID].ships) == 0 or (turn_number > 20 and not at_home):
            commands.spawn(0) # We don't want to put any halite in the newly spawned ship.

    game.end_turn(commands) # Send our moves back to the game environment
