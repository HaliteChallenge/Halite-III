// First off, here are some defines and includes. I forget what exactly needs what
#define _GNU_SOURCE

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// We include time.h so we can seed the RNG for default "smart" movement with time()
#include <time.h>


// Now let's include some helper macros
// You might reasonably want to keep some of these
#include "macros.c"


// Now for our data structures. These are intentionally left rather basic, so add whatever you want to them
struct ship{
	unsigned char x, y;
	unsigned short halite;
} ships[4096];

struct player{
	unsigned char dropoffs[64], dropoff_count, ship_count, ship_yard[2];
	unsigned short ships[1024];
	unsigned int halite;
} players[4] = {0};

struct map{
	unsigned short halite;
	unsigned short occupied; //(unsigned short)-1 means it's not occupied
} map[64][64];


// Now for some global variables to help us keep track of things
unsigned char dropoffs[256][2], mhh, mhw, my_id, map_size[2], number_of_players;
unsigned short max_turn, turn_number;

FILE *log_file;


// Now that all of the variables have been declared, we can include our default logic C file
#include "default_logic.c"
// This file can safely be removed or replaced with your own code


int main(int argc, char* argv[]){
	char *c;
	unsigned char i, id, j, sca, scb, scc;
	unsigned short s, ssa, ssb;
	unsigned int sia;

	// Firstly, we open a log file. If no log file is specified, it defaults to "MyBot.log"
	if(argc != 2)
		c = "MyBot.log";
	else
		c = argv[1];

	log_file = fopen(c, "w");

	// The first input we get from the engine is some JSON-encoded constants. This line can safely be skipped since the constants are, well, constants as far as the competition goes
	if(scanf("%*s") != 0)
		exit(1);

	// Get number of players and player ID
	if(scanf("%hhu %hhu", &number_of_players, &my_id) < 1)
		exit(1);


	// Now that we have our player ID, we add it to time(NULL) to seed our "smart" RNG
	srand(time(NULL) + my_id);


	// Get each player's ship yard point coordinates
	for(i=0; i<number_of_players; i++){
		if(scanf("%hhu %hhu %hhu", &sca, &scb, &scc) < 3)
			exit(1);

		players[sca].ship_yard[0] = scb;
		players[sca].ship_yard[1] = scc;
	}

	// Read the map size
	if(scanf("%hhu %hhu", &map_size[0], &map_size[1]) < 2)
		exit(1);

	// Now that we have the map size, let's set the map half width and map half height variables
	mhw = map_size[0] >> 1;
	mhh = map_size[1] >> 1;

	// Figure out the max turn (We could just parse this from the JSON at the start, but that would be too easy)
	switch(map_size[0]){
		case 32:
			max_turn = 400;
		break;
		case 40:
			max_turn = 425;
		break;
		case 48:
			max_turn = 450;
		break;
		case 56:
			max_turn = 475;
		break;
		case 64:
			max_turn = 500;
		break;
	}

	// Finally, we read the initial map
	for(i=0; i<map_size[0]; i++)
		for(j=0; j<map_size[1]; j++)
			if(scanf("%hu", &map[j][i].halite) < 1)
				exit(1);

	// Now that we've read the starting data, it's time to output our bot's name
	printf("MyBot\n");
	// stdout has to be flushed before the game engine will actually get the commands
	fflush(stdout);


	// Now we start the game loop
	for(;;){
		// Before we deal with the game engine, we should reset whether or not each map tile is occupied
		for(i=0; i<map_size[1]; i++)
			for(j=0; j<map_size[0]; j++)
				map[j][i].occupied = (unsigned short)-1;


		// Read the turn number
		if(scanf("%hu", &turn_number) < 1)
			exit(6);

		// Now we're going to save the turn number to the log file just to show how it's done
		fprintf(log_file, "Starting turn %hu\n", turn_number);
		fflush(log_file);

		// Read information about how each of the players are doing
		for(i=0; i<number_of_players; i++){
			if(scanf("%hhu %hhu %hhu %u", &sca, &scb, &scc, &sia) < 4)
				exit(1);

			id = sca;
			players[id].ship_count = scb;
			players[id].dropoff_count = scc;
			players[id].halite = sia;

			// Parse ships
			for(s=0; s<players[id].ship_count; s++){
				if(scanf("%hu %hhu %hhu %hu", &ssa, &sca, &scb, &ssb) < 4)
					exit(1);

				players[id].ships[s] = ssa;

				ships[ssa].x = sca;
				ships[ssa].y = scb;
				ships[ssa].halite = ssb;

				// And mark the map tile as occupied
				map[sca][scb].occupied = ssa;
			}

			// And now parse drop offs
			for(j=0; j<players[id].dropoff_count; j++){
				if(scanf("%hhu %hhu %hhu", &sca, &scb, &scc) < 3)
					exit(1);

				dropoffs[sca][0] = scb;
				dropoffs[sca][1] = scc;
				players[id].dropoffs[i] = sca;
			}
		}

		// Now, the game engine gives us the number of map tiles that have an updated halite count
		if(scanf("%hhu", &i) < 1)
			exit(1);

		// If there are any updates, we parse them and update the map
		if(i > 0)
			for(; i>0; i--){
				if(scanf("%hhu %hhu %hu", &sca, &scb, &ssa) < 3)
					exit(1);

				map[sca][scb].halite = ssa;
			}

		// Now we add logic for controlling the ships here
		// Here, we call the default move function from default_logic.c for each of our ships
		for(i=0; i<players[my_id].ship_count; i++){
			// Let's borrow ssa for the ship's ID just to make the code a little easier
			ssa = players[my_id].ships[i];
			naive_navigate(ssa);

			// Let's also show how far each ship is away from the ship yard
			fprintf(log_file, "Ship %hu is %hhu moves away from the ship yard!\n", ssa, CALC_DIST(ships[ssa].x, ships[ssa].y, players[my_id].ship_yard[0], players[my_id].ship_yard[1]));
			fflush(log_file);
		}

		// Then we add logic for when to spawn ships
		// The default "smart" logic will be to spawn a ship every 5th + 1 turn until we run out of halite
		if(turn_number % 5 == 0 && turn_number <= 50 && players[my_id].halite >= 1000)
			SPAWN_SHIP();


		// Finally, we print a new line, and then flush the stdout buffer so the game engine receives our commands
		printf("\n");
		fflush(stdout);

		// And write another log stating it's the end of the turn!
		fprintf(log_file, "Ending turn %hu\n\n", turn_number);
		fflush(log_file);
	}

	return 0;
}
