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
game.ready("MyPythonBot")

# Now that we're initialized, let's save a message to ourselves in the log file with some important information.
#   Specifically we log here our id, which we can always fetch from the game object by using my_id.
logging.info("Successfully created bot! My Player ID is {}.".format(game.my_id))

while True:
    # Now that we're set-up, we need to run the game. Since the game changes every turn (we call this a game state),
    #   we refresh that state by running update_frame (a frame is one atomic epoch of a game state)
    game.update_frame()
    me = game.me  # Here we extract our player metadata from the game state
    game_map = game.game_map  # And here we extract the map metadata

    # A command queue holds all the commands we will run this turn. We store all of them here, since we
    #   can only submit them once, hence we save all of them to be submitted at the end of turn
    command_queue = []

    for ship in me.get_ships():  # For each of our ships
        # Check if there's a reasonable amount of halite on the square.
        # If so, extract halite. If not, move randomly. If we don't move we can assume we're extracting Halite
        if game_map[ship.position].halite_amount < constants.MAX_HALITE / 10 or ship.is_full:
            # Move this ship in a random direction, picking from one of the cardinals
            command_queue.append(
                ship.move(
                    random.choice([ Direction.North, Direction.South, Direction.East, Direction.West ])))
        else:
            command_queue.append(ship.stay_still())  # Don't move

    # If we're in the first 200 turns and have enough halite, spawn a ship.
    # Don't spawn a ship if we currently have a ship at port, though.
    if game.turn_number <= 200 and me.halite_amount >= constants.SHIP_COST and not game_map[me.shipyard].is_occupied:
        command_queue.append(me.shipyard.spawn())

    game.end_turn(command_queue)  # Send our moves back to the game environment
