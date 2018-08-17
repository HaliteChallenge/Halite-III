---
layout: doc_page
title: Game Overview
toc: false
description: Get an introduction to the rules of the game to win the Halite AI Programming Challenge.
sort_key: 0
---

<div class="doc-section" markdown="1">

Halite III, codenamed Shipyard, is an exploratory resource management game, in which 2-4 players command a fleet of ships that trawl the sea for halite, a sparkling energy resource.

The object of the game is to collect and deposit the most halite into shipyards and drop-off zones, over the course of 500 turns. The winner is whoever has the most halite in their reserves on the last turn.

Gameplay takes place on a wrapping rectangular grid of a variety of sizes. Each square grid cell contains an amount of halite.

Each player starts the game with: a base, called their shipyard; and 1000 halite in their store. The shipyard's location does not move. Ships may move on top of the shipyard and dump halite there, depositing the resource into their store.

Players can spawn ships from the shipyard, for a cost of 500 halite, plus whatever amount they wish to add as an initial load.

Ships can carry up to 1000 halite as cargo, and each turn can:
* Move one square in a cardinal direction (N, S, E, W). Each move costs 10% of the halite available at their turn origin location. If two ships collide, both are destroyed; their cargo is deposited as halite on the cell collision site.
* Dump halite.
* Remain still. In this state, ships collect 25% of the available halite per turn (rounded to the nearest integer).
* Be converted into a drop-off site for 2000 halite, additional locations for halite drop-off. Ships pay no fee when using their own dropoff sites; when using another player’s dropoff, they pay a 25% tax on delivered cargo, paid to the player who constructed the site.

</div>


<div class="doc-section" markdown="1">

#### Gameplay

Halite III proceeds as follows:

In the initialization phase, bots are given the map, the location of their shipyards, and a single ship on top of their shipyard. They are also given time to initialize, and are expected to send back their name.

Each turn, bots are sent the state of the map, and additionally the states of them and their opponents. Bots return a series of the above described commands:

1. Spawn a ship from the shipyard
1. Move a specified ship one square in the specified direction: N, S, E, W.
1. Construct a drop-off point from a ship.
1. Dump a specified amount of halite. If the ship is on top of a shipyard or dropoff, the halite is transferred there. Otherwise, it is dumped back into the sea.

The game engine, upon receiving instructions from the player, will execute them in the order of dump commands, construct commands, move commands, and finally spawn commands, followed by computing the actual extraction of halite by ships. If and only if no command is issued to a ship, it will filter halite from the sea.

Bots have up to 6 seconds to issue commands each turn.

#### Win conditions

At the end of the game, players are ranked according to the last turn they were alive, then (in case of ties) by their stored halite on the last turn, then by their stored halite on the penultimate turn, and so on and so forth.

A player is "alive" on a turn if they have either a ship or enough energy to construct a ship.

The game ends after 500 turns, or when no players are alive, whichever comes first.

#### Note on game rules changes
The Two Sigma and Halite team reserves the right to make changes on game rules during the course of the game. We promise we won't do this without a very good reason that improves the competition and the fun of the game, and we will try to make any changes as backwards compatible as possible.

</div>