---
layout: doc_page
title: Basic Game Rules
toc: false
description: Get an introduction to the rules of the game to win the Halite AI Programming Challenge.
sort_key: 0
---

Halite III, codenamed Shipyard, is a game of strategy and production. Players command fleets of ships, using them to filter halite from the seas. They compete to have the most halite, and are ranked by the amount of halite collected at the end of the game (300 turns). Shipyard is played on a symmetric rectangular grid of square cells, which wraps around at both edges.

Players have both a base, called their shipyard, as well as ships. Ships carry a cargo of halite, which also serves as fuel; there is a TO BE DETERMINED cap on how much halite a ship can carry at a given time. They spend halite to move around the board along cardinal directions (N/S/E/W), one cell at a time; this costs them fuel/halite equal to 5% of the halite present at the location they are moving off of. If they instead remain still, they collect halite from the cell they are on, collecting 10% of the available halite per turn (rounded to the nearest integer). Ships may also choose to dump halite into the sea. Each player starts with a single ship, located at their shipyard. If two ships collide, both are destroyed; their cargo is deposited as halite on the cell they tried to move to.

The shipyard is at a fixed location and does not move. Ships may move on top of the shipyard; if they dump halite onto this location, the halite is collected and the player may spend it. They spend TO BE DETERMINED halite to construct a ship, plus however much halite they choose to put as the initial fuel/cargo level.

Additionally, players can spend a large amount of halite (TO BE DETERMINED) to convert a ship to a drop-off site (dropoff for short), giving another place where ships can deposit collected halite, in order to increase the efficiency of the others sailing the halite seas. Ships pay no fee when using their own dropoff sites; when using another player’s dropoff, they pay a 25% tax on delivered cargo, paid to the player who constructed the site.

## Map at Game Start

Halite III proceeds as follows:

In the initialization phase, bots are given the map, the location of their shipyards, and a single ship on top of their shipyard. They are also given time to initialize, and are expected to send back their name.

Each turn, bots are sent the state of the map, and additionally the states of them and their opponents. Bots return a series of commands, of which there are four types.

1. Spawn a ship from the factory with a specified amount of halite. Note that this will cost an amount greater than the specified halite, for ships have a base cost, too.
1. Move a specified ship one square in the specified direction.
1. Construct a dropoff point from a ship.
1. Dump a specified amount of halite. If the ship is on top of a shipyard or dropoff, the halite is transferred there. Otherwise, it is dumped back into the sea.

The game engine, upon receiving instructions from the player, will execute them in the order of dump commands, construct commands, move commands, and finally spawn commands, followed by computing the actual extraction of halite by ships. If two ships attempt to move to the same location, both are sunk and their cargo is lost. If and only if no command is issued to a ship, it will filter halite from the sea.

The cost of moving a ship is 5% of the halite on the present site and is taken from the ship’s onboard store of halite (rounded to the nearest integer). Additionally, ships extract 10% of the halite on a site per turn when actively filtering (also rounded to the nearest integer).

## Starting a game

Two or four players compete in a match.
At the start of the game, each player will have 3 ships on the map.

## Win conditions
The only win conditions are:

1. All other players have been eliminated from the game (through bot errors), or
1. You have the most halite stocked in your shipyard & dropoffs at the end of the 300 turns.

## Note on game rules changes
The Two Sigma and Halite team reserves the right to make changes on game rules during the course of the game. We promise we won't do this without a very good reason that improves the competition and the fun of the game, and we will try to make any changes as backwards compatible as possible.
