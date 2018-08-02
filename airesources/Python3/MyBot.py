#!/usr/bin/env python3

import hlt
import random # Needed for choosing a random direction

game = hlt.Game()
# Get the initial game state
game_map, players, my_id = game.get_init()
# Respond with our name
game.send_init("MyPythonBot-" + str(my_id))

while True:
    # Get the latest game state
    turn_number, game_map, players = game.get_frame()

    me = players[my_id]
    command_queue = []
    safe_mover = hlt.SafeMover(game_map, players)

    at_home = False
    for ship in me.ships.values(): # For each of our ships
        # If we're on our shipyard and have enough halite, dump it.
        if ship.location == me.shipyard.location and ship.halite > hlt.MAX_HALITE / 4:
            command_queue.append(ship.dump(ship.halite))
            at_home = True
        # Otherwise, check if there's a reasonable amount of halite on the square.
        # If so, extract halite. If not, move randomly.
        elif game_map[ship.location].halite > hlt.MAX_HALITE / 10 and not ship.is_full:
            pass # Do nothing, which is to say, extract halite.
        else:
            # Move in a random direction
            command_queue.append(safe_mover.move(ship, random.choice(["n", "e", "s", "w"])))

    # If we're in the first 200 turns and have enough halite, spawn a ship.
    # Don't spawn a ship if we currently have a ship at port, though.
    if turn_number <= 200 and me.halite >= hlt.SHIP_COST and not at_home:
        # We don't want to put any halite in the newly spawned ship.
        command_queue.append(me.shipyard.spawn(0))

    game.end_turn(command_queue) # Send our moves back to the game environment
