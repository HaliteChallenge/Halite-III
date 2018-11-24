// First off, here are some defines and includes. I forget what exactly needs what
#include <stdio.h>
#include <stdlib.h>

// We include time.h so we can seed the RNG for default "smart" movement with time()
#include <time.h>



// Now for our data structures. These are intentionally left rather basic, so add whatever you want to them
struct ship{
	unsigned char x, y;
	unsigned short halite;
} ships[4096];

struct player{
	unsigned char dropoffs[64], dropoff_count, ship_count, ship_yard[2];
	unsigned short ships[1024];
	unsigned int halite;
} players[4];

struct map{
	unsigned short halite;
	unsigned short occupied; //(unsigned short)-1 means it's not occupied
} map[64][64];



// Now for some global variables to help us keep track of things
unsigned char dropoffs[256][2], extract_ratio, inspiration_enabled, inspiration_radius, inspiration_ship_count, inspired_extract_ratio, inspired_bonus_multiplier, inspired_move_cost_ratio, mhh, mhw, move_cost_ratio, my_id, map_size[2], number_of_players;
unsigned short dropoff_cost, max_energy, max_turns, ship_cost, turn_number;

FILE *log_file = NULL;



// Now let's declare some helper functions
// You might reasonably want to keep some of these

// Normalize X value (map_width + 1 = 0, 0 - 1 = map_width)
inline unsigned char norm_x(char x){
	return (x + map_size[0]) % map_size[0];
}

// Normalize Y value (map_height + 1 = 0, 0 - 1 = map_height)
inline unsigned char norm_y(char y){
	return (y + map_size[1]) % map_size[1];
}

// Calculate the distance betwen two sets of x and y values
inline unsigned char calc_dist(unsigned char ax, unsigned char ay, unsigned char bx, unsigned char by){
	unsigned char dx, dy;

	if(ax > bx)
		dx = ax - bx;
	else
		dx = bx - ax;

	if(ay > by)
		dy = ay - by;
	else
		dy = by - ay;

	return (dx % mhw) + (dy % mhh);
}

// 0 if not occupied, 1 if occupied
inline unsigned char tile_occupied(unsigned char x, unsigned char y){
	return map[x][y].occupied != (unsigned short)-1;
}

// Now for the commands to send to the game engine
inline void move_still(unsigned short ship_id){
	printf("m %hu o ", ship_id);
}

inline void move_north(unsigned short ship_id){
	printf("m %hu n ", ship_id);
}

inline void move_south(unsigned short ship_id){
	printf("m %hu s ", ship_id);
}

inline void move_east(unsigned short ship_id){
	printf("m %hu e ", ship_id);
}

inline void move_west(unsigned short ship_id){
	printf("m %hu w ", ship_id);
}


inline void convert_dropoff(unsigned short ship_id){
	printf("c %hu ", ship_id);
}

inline void spawn_ship(){
	printf("g ");
}



// Now that all of the variables have been declared, we can write some basic "smart" logic
void random_move(unsigned short ship_id){
	unsigned char decision = rand() % 100;

	// 10% chance each to move north/east/south/west (This is extra smart)
	if(decision <= 10){
		move_north(ship_id);
	}else if(decision <= 20){
		move_east(ship_id);
	}else if(decision <= 30){
		move_south(ship_id);
	}else if(decision <= 40){
		move_west(ship_id);
	}else{
		// And a 60% chance to stay still (This is also extra smart)
		// We could alternatively just not give the ship a move this turn
		move_still(ship_id);
	}
}


// There's something that random_move is forgetting... Let's avoid collisions!
void naive_navigate(unsigned short ship_id){
	unsigned char decision = rand() % 100, x = ships[ship_id].x, y = ships[ship_id].y;

	// North is y - 1
	// South is y + 1
	// West is x - 1
	// East is x + 1

	// 10% chance each to move north/east/south/west (This is even smarter than before!)
	if(decision <= 10 && tile_occupied(x, norm_y(y - 1)) == 0){
		move_north(ship_id);
	}else if(decision <= 20 && tile_occupied(norm_x(x + 1), y) == 0){
		move_east(ship_id);
	}else if(decision <= 30 && tile_occupied(x, norm_y(y + 1)) == 0){
		move_south(ship_id);
	}else if(decision <= 40 && tile_occupied(norm_x(x - 1), y) == 0){
		move_west(ship_id);
	}else{
		// And a 60% chance to stay still (This is also smarter than before!)
		move_still(ship_id);
	}
}
// This file can safely be removed or replaced with your own code



int main(int argc, char* argv[]){
	char *c, inspiration_enabled_str[6];
	unsigned char i, id, j, sca, scb, scc;
	unsigned short s, ssa, ssb;
	unsigned int sia;

	// Firstly, we open a log file. If no log file is specified, it defaults to "MyBot.log"
	if(argc != 2){
		c = "MyBot.log";
	}else{
		c = argv[1];
	}

	log_file = fopen(c, "w");

	fprintf(log_file, "Reading JSON\n");
	fflush(log_file);

	// The first input we get from the engine is some JSON-encoded constants. I'm just going to scanf this becuase the formatting is consistent
	if(scanf("{\"CAPTURE_ENABLED\":%*5[^,],\"CAPTURE_RADIUS\":%*u,\"DEFAULT_MAP_HEIGHT\":%*u,\"DEFAULT_MAP_WIDTH\":%*u,\"DROPOFF_COST\":%hu,\"DROPOFF_PENALTY_RATIO\":%*u,\"EXTRACT_RATIO\":%hhu,\"FACTOR_EXP_1\":%*f,\"FACTOR_EXP_2\":%*f,\"INITIAL_ENERGY\":%*u,\"INSPIRATION_ENABLED\":%5[^,],\"INSPIRATION_RADIUS\":%hhu,\"INSPIRATION_SHIP_COUNT\":%hhu,\"INSPIRED_BONUS_MULTIPLIER\":%hhu.0,\"INSPIRED_EXTRACT_RATIO\":%hhu,\"INSPIRED_MOVE_COST_RATIO\":%hhu,\"MAX_CELL_PRODUCTION\":%*u,\"MAX_ENERGY\":%hu,\"MAX_PLAYERS\":%*u,\"MAX_TURNS\":%hu,\"MAX_TURN_THRESHOLD\":%*u,\"MIN_CELL_PRODUCTION\":%*u,\"MIN_TURNS\":%*u,\"MIN_TURN_THRESHOLD\":%*u,\"MOVE_COST_RATIO\":%hhu,\"NEW_ENTITY_ENERGY_COST\":%hu,\"PERSISTENCE\":%*f,\"SHIPS_ABOVE_FOR_CAPTURE\":%*u,\"STRICT_ERRORS\":%*5[^,],\"game_seed\":%*u}", &dropoff_cost, &extract_ratio, (char *)&inspiration_enabled_str, &inspiration_radius, &inspiration_ship_count, &inspired_bonus_multiplier, &inspired_extract_ratio, &inspired_move_cost_ratio, &max_energy, &max_turns, &move_cost_ratio, &ship_cost) == 255){
		exit(1);
	}

	if(inspiration_enabled_str[0] == 't')
		inspiration_enabled = 1;
	else
		inspiration_enabled = 0;


	// Get number of players and player ID
	if(scanf("%hhu %hhu", &number_of_players, &my_id) < 1){
		exit(1);
	}

	// Now that we have our player ID, we add it to time(NULL) to seed our "smart" RNG
	srand(time(NULL) + my_id);

	// Get each player's ship yard point coordinates
	for(i = 0; i < number_of_players; i++){
		if(scanf("%hhu %hhu %hhu", &sca, &scb, &scc) < 3){
			exit(1);
		}

		players[sca].ship_yard[0] = scb;
		players[sca].ship_yard[1] = scc;
	}

	// Read the map size
	if(scanf("%hhu %hhu", &map_size[0], &map_size[1]) < 2){
		exit(1);
	}

	// Now that we have the map size, let's set the map half width and map half height variables
	mhw = map_size[0] >> 1;
	mhh = map_size[1] >> 1;

	// Finally, we read the initial map
	for(i = 0; i < map_size[0]; i++){
		for(j = 0; j < map_size[1]; j++){
			if(scanf("%hu", &map[j][i].halite) < 1){
				exit(1);
			}
		}
	}

	// Now that we've read the starting data, it's time to output our bot's name
	printf("MyBot\n");
	// stdout has to be flushed before the game engine will actually get the commands
	fflush(stdout);


	// Now we start the game loop
	while(1){
		// Before we deal with the game engine, we should reset whether or not each map tile is occupied
		for(i = 0; i < map_size[1]; i++){
			for(j = 0; j < map_size[0]; j++){
				map[j][i].occupied = (unsigned short)-1;
			}
		}


		// Read the turn number
		if(scanf("%hu", &turn_number) < 1){
			exit(1);
		}

		// Now we're going to save the turn number to the log file just to show how it's done
		fprintf(log_file, "Starting turn %hu\n", turn_number);
		fflush(log_file);

		// Read information about how each of the players are doing
		for(i = 0; i < number_of_players; i++){
			if(scanf("%hhu %hhu %hhu %u", &sca, &scb, &scc, &sia) < 4){
				exit(1);
			}

			id = sca;
			players[id].ship_count = scb;
			players[id].dropoff_count = scc;
			players[id].halite = sia;

			// Parse ships
			for(s = 0; s < players[id].ship_count; s++){
				if(scanf("%hu %hhu %hhu %hu", &ssa, &sca, &scb, &ssb) < 4){
					exit(1);
				}

				players[id].ships[s] = ssa;

				ships[ssa].x = sca;
				ships[ssa].y = scb;
				ships[ssa].halite = ssb;

				// And mark the map tile as occupied
				map[sca][scb].occupied = ssa;
			}

			// And now parse drop offs
			for(j = 0; j < players[id].dropoff_count; j++){
				if(scanf("%hhu %hhu %hhu", &sca, &scb, &scc) < 3){
					exit(1);
				}

				dropoffs[sca][0] = scb;
				dropoffs[sca][1] = scc;
				players[id].dropoffs[i] = sca;
			}
		}

		// Now, the game engine gives us the number of map tiles that have an updated amount of halite
		if(scanf("%hhu", &i) < 1){
			exit(1);
		}

		// If there are any updates, we parse them and update the map
		for(; i > 0; i--){
			if(scanf("%hhu %hhu %hu", &sca, &scb, &ssa) < 3){
				exit(1);
			}

			map[sca][scb].halite = ssa;
		}

		// Now we add logic for controlling the ships here
		// Here, we call the default move function from default_logic.c for each of our ships
		for(i = 0; i < players[my_id].ship_count; i++){
			// Let's borrow ssa for the ship's ID just to make the code a little easier
			ssa = players[my_id].ships[i];
			naive_navigate(ssa);

			// Let's also show how far each ship is away from the ship yard
			fprintf(log_file, "Ship %hu is %hhu moves away from the ship yard!\n", ssa, calc_dist(ships[ssa].x, ships[ssa].y, players[my_id].ship_yard[0], players[my_id].ship_yard[1]));
			fflush(log_file);
		}

		// Then we add logic for when to spawn ships
		// The default "smart" logic will be to spawn a ship every 5th turn until we run out of halite or pass turn 50
		if(turn_number % 5 == 0 && turn_number <= 50 && players[my_id].halite >= 1000){
			spawn_ship();
		}


		// Finally, we print a new line, and then flush the stdout buffer so the game engine receives our commands
		printf("\n");
		fflush(stdout);

		// And write another log stating it's the end of the turn!
		fprintf(log_file, "Ending turn %hu\n\n", turn_number);
		fflush(log_file);
	}

	return 0;
}
