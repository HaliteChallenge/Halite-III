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

Players begin play with a shipyard, and can use collected halite to build new ships. To efficiently collect halite, players must devise a strategy based on building ships and dropoff points. Ships can explore the ocean, collect halite, and store it in the shipyard or in dropoff points. Players interact by seeking inspiring competition, or by colliding ships to send both to the bottom of the sea.
<br/><br/>
<span style="display:block;text-align:center">![a centered gif showing wrapping map grid edges and collision](/assets/images/map_inspiration.gif)</span>
</div>

<div class="doc-section" markdown="1">

#### Gameplay Overview

Players each start the game with 5,000 stored halite, a shipyard, and knowledge of the game map. Players play in groups of two or four on a 2d map (32x32, 40x40, 48x48, 56x56, or 64x64) with a unique symmetric pattern of halite.

Ships can make one action per turn: they can move one unit in any cardinal direction, collect halite, or convert into dropoffs. When a ship is over a friendly shipyard or dropoff, it automatically deposits its halite cargo, adding to the player's total halite.

Ships interact directly in two ways. If multiple ships occupy the same location, they will **collide** and sink, dropping all their halite into the sea. If there are two or more ships belonging to any opponent within a four-cell radius of your ship, your ship is **inspired** by the competition. An inspired ship collects halite from the sea at the normal rate, but receives an additional 200% bonus.

Each turn, the game engine sends the players the positions of all ships and dropoffs, and an updated game map. Players have up to two seconds to issue their commands for the turn. The game engine will parse and execute the commands, calculating each player’s resulting halite score and resolving all movement.

Players spend halite to **build a ship**,  **move a ship**, and to **convert a ship to a dropoff**. Players gain halite by **collecting halite** from the sea in their current position.
<br/><br/>

Command | Halite Cost or Gain
:--- | :--- | :---
Spawn | Cost: 1000 halite
Convert into a drop-off  | Cost: 4000 halite deducted from player's stored halite. <br/> The converted ship's halite cargo and the halite in the sea under the new dropoff is credited to the player, potentially reducing the cost.
Move: North, South, East, West | Cost: 10% of halite available at turn origin cell is deducted from ship’s current halite. <br/>When a ship moves over a friendly shipyard or dropoff, it deposits its halite cargo.
Move: Stay still | Gain: 25% of halite available in cell, rounded up to the nearest whole number. Ship remains at its origin. <br/>Ships can carry up to 1000 halite.

<br/><br/>

##### Win Conditions
The game continues for 400 to 500 turns, depending on the game map size. The winning player has the most stored halite at the end of the game. If a player does not have a ship or enough energy to construct a ship, they are considered “dead” and lose the game. In the case of a tie, players are ranked by their stored halite on the last turn, then by their stored halite on the penultimate turn, and so on.
<br/><br/>

##### Note on Game Rules Changes
The Two Sigma and Halite team reserves the right to make changes on game rules during the course of the game. We promise we won’t do this without a very good reason that improves the competition and the fun of the game, and we will try to make any changes as backwards compatible as possible.


</div>

<div class="build-a-bot text-center">
  <a href="/learn-programming-challenge/tutorials" class="btn btn-primary">Build a bot</a>
</div>
