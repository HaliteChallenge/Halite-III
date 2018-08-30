#!/usr/bin/env python3

# Here we import the Halite SDK. This library will help you interact with the game
import hlt

# This library contains constant values throughout the game. Things like how much halite can your ship carry, etc.
from hlt import constants

# This library contains direction metadata to better interface with the game
from hlt.positionals import Direction

# This library allows us to generate random numbers
import random

# Logging allows us to save messages for ourselves. It is how the bot communicates with us. This is required
#   as the bot talks to the game via regular STDOUT (print statements), so we can't use that channel otherwise the
#   game will get confused.
# Log files will be saved wherever you run the bot, under the name bot-*, where * would be your bot name.
import logging

""" <<<Game Begin>>> """

# Creates a bot with the name MyPythonBot, registering for the game
# This game object contains the start game state
game = hlt.Game()
game.ready("PriorityCollector")

# Now that we're initialized, let's save a message to ourselves in the log file with some important information.
#   Specifically we log here our id, which we can always fetch from the game object by using my_id.
logging.info("Successfully created bot! My Player ID is {}.".format(game.my_id))
logging.info("{}".format(hlt.constants.SHIP_COST))

state = {}

while True:
    # Now that we're set-up, we need to run the game. Since the game changes every turn (we call this a game state),
    #   we refresh that state by running update_frame (a frame is one atomic epoch of a game state)
    game.update_frame()
    me = game.me  # Here we extract our player metadata from the game state
    game_map = game.game_map  # And here we extract the map metadata

    # Process map to find high-density regions
    cells = []
    for y_base in range(0, game_map.height, 8):
        for x_base in range(0, game_map.width, 8):
            total = 0
            best = (x_base, y_base)
            best_total = 0
            for y in range(y_base, y_base + 8):
                for x in range(x_base, x_base + 8):
                    cur_halite = game_map[hlt.Position(x, y)].halite_amount
                    total += cur_halite
                    if cur_halite > best_total:
                        best = (x, y)
                        best_total = cur_halite

            if total > 3000:
                cells.append((best[0], best[1], total / (1 + game_map.calculate_distance(hlt.Position(x_base + 4, y_base + 4), me.shipyard.position))))
    best_cell = max(cells, key=lambda x: x[2])

    # A command queue holds all the commands we will run this turn. We store all of them here, since we
    #   can only submit them once, hence we save all of them to be submitted at the end of turn
    command_queue = []

    for ship in me.get_ships():  # For each of our ships
        # Check if there's a reasonable amount of halite on the square.
        # If so, extract halite. If not, move randomly. If we don't move we can assume we're extracting Halite
        if ship.position == me.shipyard.position:
            state[ship.id] = None

        if state.get(ship.id) == 'return' or ship.halite_amount > 700:
            state[ship.id] = 'return'
            destination = me.shipyard.position
            safe_move = game_map.get_safe_move(game_map[ship], game_map[destination])
            if safe_move:
                game_map[ship.position.directional_offset(safe_move)].mark_unsafe(ship)
                command_queue.append(ship.move(safe_move))
        elif game_map[ship.position].halite_amount < constants.MAX_HALITE / 10:
            # Move this ship in a random direction, picking from one of the cardinals
            target_pos = hlt.Position(best_cell[0], best_cell[1])
            safe_move = game_map.get_safe_move(game_map[ship], game_map[target_pos])
            if not safe_move:
                direction = random.choice([ Direction.North, Direction.South, Direction.East, Direction.West ])
                destination = ship.position.directional_offset(direction)
                safe_move = game_map.get_safe_move(game_map[ship], game_map[destination])
            if safe_move:
                game_map[ship.position.directional_offset(safe_move)].mark_unsafe(ship)
                command_queue.append(ship.move(safe_move))
        else:
            command_queue.append(ship.stay_still())  # Don't move

    # If we're in the first 200 turns and have enough halite, spawn a ship.
    # Don't spawn a ship if we currently have a ship at port, though.
    if len(me.get_ships()) < 15 and game.turn_number <= 200 and me.halite_amount >= constants.SHIP_COST and not game_map[me.shipyard].is_occupied:
        command_queue.append(game.me.shipyard.spawn())

    game.end_turn(command_queue)  # Send our moves back to the game environment
