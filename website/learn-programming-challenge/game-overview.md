---
layout: doc_page
title: Game Overview
breadcrumb: Learn
toc: false
description: Get an introduction to the rules of the game to win the Halite AI Programming Challenge.
sort_key: 1
---

<div class="doc-section" markdown="1">

#### Game Rules

Halite III is a resource management game in which players build and command ships that explore the ocean and collect halite. Ships use halite as an energy source, and the player with the most stored halite at the end of the game is the winner.

Players begin play with a shipyard, and can use collected halite to build new ships. To efficiently collect halite, players must devise a strategy based on building ships and dropoff points. Ships can explore the ocean, collect halite, and store it in the shipyard or in dropoff points. Players interact by seeking inspiring competition, capturing ships with precise formations, and crashing ships to send both to the bottom of the sea.
<br/><br/>
<span style="display:block;text-align:center">![a centered gif showing wrapping map grid edges and collision](/assets/images/map_inspiration.gif)</span>
</div>

<br/><br/>

<div class="doc-section" markdown="1">

#### Gameplay Overview

Players each start the game with 5,000 stored halite, a shipyard, and knowledge of the game map. The map is a 2d grid (32x32, 48x48, 64x64, or 80x80) with a unique symmetric pattern of halite. Each player's shipyard position is fixed on the map, and the shipyard is the only place a player can spawn new ships.

Ships are the main actors in a game of Halite III. Ships explore the board, collect precious halite, and can interact with opposing players. Ships are powered by the energy they collect and store, and any move costs halite. Ships can make one action per turn: they can move one unit in any cardinal direction, collect halite, or convert into dropoffs.

Ships interact in a number of ways. If two ships occupy the same location, they will sink, dropping all their halite into the sea. An outnumbered ship can be inspired by the competition, and can get an efficiency bonus to collecting halite when in the inspired state. A ship can be captured and change owners if it is flanked by an opposing player’s ships.

Each turn, the game engine sends the players the positions of all ships and dropoffs, along with a new game map with updated values that changed on the previous turn. Players have up to two seconds to issue their commands for the turn. The game engine will parse and execute the commands, calculating each player’s resulting halite score and resolving all movement.

Players spend halite to **build a ship**,  **move a ship**, and to **construct a dropoff**. Players gain halite by **collecting halite** from the sea in their current position.
<br/><br/>

Command|Halite Cost or Gain
:---: | :---: | :---:
Spawn | Spend: 1000 halite
Construct a drop-off  | Spend: 2000 halite
Move (n, s, e w) | Spend: 10% of halite available at turn origin cell is deducted from ship’s current halite
Move (o) | Collect: 25% of halite available in cell. Ships can carry up to 1000 halite.

<br/><br/>
##### Interacting with Other Ships
If your ship is outnumbered by three or more ships belonging to a single opponent within a three-cell radius, your ship is **captured**, and changes owners to the flanking player.


If there are two or more ships belonging to any opponent within a four-cell radius of your ship, your ship is **inspired**, and collects halite with an efficiency bonus.
<br/><br/>

##### Win Conditions
The game continues for 300 to 500 turns, depending on the game map size. The winning player has the most stored halite at the end of the game. If a player does not have a ship or enough energy to construct a ship, they are considered “dead” and lose the game. In the case of a tie, players are ranked by their stored halite on the last turn, then by their stored halite on the penultimate turn, and so on.
<br/><br/>

##### Note on Game Rules Changes
The Two Sigma and Halite team reserves the right to make changes on game rules during the course of the game. We promise we won’t do this without a very good reason that improves the competition and the fun of the game, and we will try to make any changes as backwards compatible as possible.


</div>

<div class="build-a-bot text-center">
  <a href="#" class="btn btn-primary">Build a bot</a>
</div>
