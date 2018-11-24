# C starter kit.

Don't expect very much abstraction, and expect to get your hands quite dirty, this is C after all. Here is some documentation to give you a basic run down of what is happening:


## GLOBAL VARIABLES
* **ships** An array of all ships
* **dropoffs** An array of all dropoffs (x/y coordinates)
* **players** An array of all players
* **my_id** The ID in the players array for you
* **number_of_players** The... number of players?
* **players.ships** Array of IDs for ships owned by player
* **players.dropoffs** Array of IDs for dropoffs owned by player
* **map** An array of all map tiles
* **map_size** [0] is width, [1] is height
* **mhh** Map half height. Useful for determining distance
* **mhw** Map half width. Useful for determining distance
* **max_turn** The last turn to give commands
* **turn_number** The current turn number
* **log_file** The file descriptor for the log file


## FUNCTION REFERENCE
* **norm_x** Normalizes the X coordinate (0 - 1 = map_width, map_width + 1 = 0)
* **norm_y** Normalizes the Y coordinate (0 - 1 = map_height, map_height + 1 = 0)
* **calc_dist** Calculates the distance between two sets of X and Y values
* **tile_occupied** 1 if the tile has a ship on it, otherwise 0
* **move_still** Move ship direction o, which keeps you ship on the same tile
* **move_north** Move ship direction n, North
* **move_south** Move ship direction s, South
* **move_east** Move ship direction e, East
* **move_west** Move ship direction w, West
* **convert_dropoff** Converts the ship into a dropoff
* **spawn_ship** Creates a ship in your ship yard
* **random_move** Moves the ship in a random direction
* **naive_navigate** Same as random_move, but avoid collisions


## BUILD INSTRUCTIONS

Simply cd into the src directory and type `cmake .` and then `make`


## NOTES

* Your bot name and all turns must end with a \n new line character, or else your bot will fail to initialize or move.
* The default "smart" logic isn't very smart.
* https://halite.io/learn-programming-challenge/api-docs is the definitive reference on game rules.