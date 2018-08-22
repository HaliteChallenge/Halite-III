#!/usr/bin/env python3

import hlt

game = hlt.Game()
# Get the initial game state
game_map, players, my_id = game.get_init()
# Respond with our name
game.send_init("Retriever-" + str(my_id))

while True:
    # Get the latest game state
    turn_number, game_map, players = game.get_frame()

    me = players[my_id]
    command_queue = []
    safe_mover = hlt.SafeMover(game_map, players)

    at_home = False
    # For each of our ships
    for ship in me.ships.values():
        # If we're on our shipyard and have enough halite, dump it.
        if ship.location == me.shipyard.location and ship.halite > 0:
            command_queue.append(ship.dump(ship.halite))
            at_home = True
        # If we have lots of halite, return home.
        elif ship.halite > hlt.MAX_HALITE / 2:
            command_queue.append(safe_mover.move_towards(ship, me.shipyard.location))
        # Otherwise, check if there's a reasonable amount of halite on the square.
        # If so, extract halite.
        elif game_map[ship.location].halite > hlt.MAX_HALITE / 10 and not ship.is_full:
            pass # Do nothing, which is to say, extract halite.
        else:
            # Move towards a nearby high production zone

            best_location = None
            for dx in range(-10, 10):
                for dy in range(-10, 10):
                    loc = ship.location.with_offset(dx, dy)
                    if not best_location or game_map[loc].halite > game_map[best_location].halite:
                        best_location = loc

            command_queue.append(safe_mover.move_towards(ship, best_location))

    # If we're in the first 300 turns and have enough halite, spawn a ship.
    # Don't spawn a ship if we currently have a ship at port, though.
    if turn_number <= 300 and len(me.ships) < 30 \
       and me.halite >= hlt.SHIP_COST and not at_home:
        # We don't want to put any halite in the newly spawned ship.
        command_queue.append(me.shipyard.spawn(0))

    game.end_turn(command_queue) # Send our moves back to the game environment
