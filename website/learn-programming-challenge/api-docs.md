---
layout: doc_page
title: API Documentation
breadcrumb: Learn
toc: false
description: Learn about the Halite III API
sort_key: 3
---

<div class="doc-section" markdown="1">

#### Overview

This API Documentation refers to objects shipped with Halite III starter kits:
* [GAME](#game)
* [PLAYER](#player)
* [SHIP](#ship)
* [SHIPYARD](#shipyard)
* [DROPOFF](#dropoff)
* [MAP](#map)
* [DIRECTIONS, POSITIONS, & MAP CELLS](#directions-positions-and-map-cells)

<br/>
#### Game

  The game object holds all metadata to run the game, and is an organizing layer between your code and the game engine. Game initializes the game, which includes generating the map and registering the players.

<br/>
  * **Initialization phase**

     A game of Halite III is initialized when each player sends a string name. Game forwards this to the engine, and launches the game.

     `game.ready(“name”)`

<br/>
  * **Game loop**

     The game loop sends the game state to the players and processes commands returned from the players. This repeats for each turn. Games last between 300 and 500 turns per game depending on map size. If your code takes more than 2,000 milliseconds to process, the game engine kills your bot.

     `game.update_frame()` updates the game state, and returns nothing.

<br/>
  * **Command queue**

     The command queue is a list of commands. The player’s code fills this list with commands and sends it to the game object, where it is sent to the game engine. If there are multiple commands to one ship, the game engine kills your bot.

     `game.end_turn([commands])`

     Valid commands that can be sent to the engine:

     | Action | Engine Command |
     | ---|--- |
     | GENERATE | `g` |
     | CONSTRUCT | `c` |
     | MOVE | `m` |
     | Move Commands | `n`, `s`, `e`, `w` and `o` for origin (stay still) |


<br/>
#### PLAYER
Players have an `id`, a `shipyard`, a `halite_amount`, and dictionaries of ships and dropoffs as member variables.

<br/>
* **Ships**

   Players can access their ships either singly by id, or all together as a list. See methods on ships below.

   `player.get_ship(ship_id)` returns the ship object associated with the ship id provided as an argument.

   `player.get_ships()` returns a list of all ship objects

   `player.has_ship(ship_id)` checks if you have a ship with this id.

<br/>
* **Dropoffs**

   Players can access their dropoffs either singly by id, or all together as a list.

   `player.get_dropoff(dropoff_id)` returns the dropoff object associated with the dropoff id provided as an argument.

   `player.get_dropoffs()` returns a list of all dropff objects.

<br/>
#### SHIP
Ships carry up to 1,000 halite as cargo and can be issued one command per turn via the command queue. Ships automatically deposit their cargo when over the shipyard or dropoff points. If two ships collide, both are destroyed; their cargo falls back into the sea at the collision site.

Ships have an `owner`, an `id`, a `position`, and a `halite_amount`.

<br/>
* **Cargo**

   `ship.is_full` returns a boolean True if ship is carrying 1,000 halite (the maximum). Otherwise returns False.

<br/>
* **Convert to Dropoff**

   Ships can be converted into dropoff sites at their present location to facilitate halite collection. The conversion costs 4,000 halite, deducted from total current stored halite. The converted ship's halite cargo is destroyed, and the halite amount in the sea under the new dropoff is set to 0.

   `ship.make_dropoff()` returns an engine command to convert this ship into a dropoff.

<br/>
* **Collect Halite at Origin**

   Ships can collect 25% of the halite from the sea at their present location, rounded up to the nearest integer.

   `ship.stay_still()` returns an engine command to keep this ship where it is and collect halite.

<br/>
* **Move**

   Ships can move one square in a cardinal direction per turn. Each move costs 10% of the halite available in the sea at the ship’s starting location, debited from the ships’ cargo. The direction of the move is communicated via the command queue.

   `ship.move(direction)` returns an engine command to move this ship in a direction without checking for collisions.

<br/>
#### SHIPYARD
Each player begins the game with a shipyard. Shipyards have an `owner`, an `id`, and a `position`.

<br/>
* **Spawn**

   `shipyard.spawn()` returns an engine command to generate a new ship.


<br/>
#### DROPOFF
You create a dropoff at any location on the map by converting a ship. Ships can store halite at a dropoff point just as they would at the shipyard. If two dropoffs are constructed in the same location, the engine returns an error and the construct command fails. The player class has the methods to access dropoffs.

Dropoffs have an `owner`, an `id`, and a `position`.

<br/>
#### MAP
Gameplay takes place on a wrapping rectangular grid 32x32, 48x48, 64x64, or 80x80 in dimension. The map edges wrap to their opposite edge and create a torus shape. The game map can be indexed by a position or by a contained entity (ship, shipyard, or dropoff). The game map has `width` and `height` as member variables.

<br/>
* **Calculate distance**

   A method that computes the Manhattan distance between two locations, and accounts for the toroidal wraparound.

   `gamemap.calculate_distance(source, target)` returns a number.

<br/>
* **Normalize position**

   A method that normalizes a position within the bounds of the toroidal map. Useful for handling the wraparound math on x and y.

   `gamemap.normalize(position)` returns a normalized position.

<br/>
* **Get Unsafe Moves**

   A method that returns a list of direction(s) to move closer to a target disregarding collision possibilities. Returns an empty list if the source and destination are the same.

   `gamemap.get_unsafe_moves(source, destination)` returns a list of closest directions toward the given target.

<br/>
* **Naive Navigate**

  A method that returns a direction to move closer to a target without colliding with other entities. Returns a direction of “still” if no such move exists.

  `gamemap.naive_navigate(ship, destination)` returns a single valid destination toward a given target.

<br/>

---
#### Directions, Positions, and Map Cells
Methods take and return map cells, positions, or directions. Some methods also take references to whole objects, for example a ship or ship.id versus a game_map[ship.id].position. It’s important to know the difference.

* A *map cell* is an object inside the game map, and has detailed information about what is in this position on the map.
* A *position* is an object with x and y values indicating the absolute position on the game map.
* A *direction* is an (x, y) tuple indicating the direction of movement from an origin cell.

---


<br/>
##### MAP CELL
A map cell is an object representation of a cell on the game map. Map cell has `position`, `halite_amount`, `ship`, and `structure` as member variables.

<br/>
* **Property Accessors**

   Map cell has property accessors that you can use to get information.

  `map_cell.is_empty` returns True if the cell is empty.

  `map_cell.is_occupied` returns True if there is a ship on this cell.

  `map_cell.has_structure` returns True if there is a structure (a dropoff or shipyard) on this cell.

  `map_cell.structure_type` returns the type of structure on this cell, or None if there is no structure.

<br/>
* **Navigational Marking**

   `map_cell.mark_unsafe(ship)` is used to mark the cell under this ship as unsafe (occupied) for collision avoidance. This marking resets every turn and is used by `naive_navigate` to avoid collisions.

<br/>
##### POSITION
A position is an object with x and y values indicating the absolute position on the game map. Position is defined in the file hlt/positionals.py.

  `position.directional_offset(direction)` returns a new position based on moving one unit in the given direction from the given position.

  `position.get_surrounding_cardinals()` returns a list of all positions around the given position in each cardinal direction.

<br/>
##### DIRECTION
A direction is an (x, y) tuple indicating the direction of movement from an origin cell. Direction is defined in the file hlt/positionals.py.

  `Direction.get_all_cardinals()` returns an array of all cardinal tuples.

  `Direction.convert()` returns a letter command from a direction tuple.

  `Direction.invert()` returns a letter command of the opposite cardinal direction given a direction tuple.

<br/>

</div>

<div class="build-a-bot text-center">
  <a href="#" class="btn btn-primary">Build a bot</a>
</div>
