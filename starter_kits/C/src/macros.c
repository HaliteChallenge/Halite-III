#define NORM_X(X)	((X) + map_size[0]) % map_size[0]
#define NORM_Y(Y)	((Y) + map_size[1]) % map_size[1]


#define TILE_OCCUPIED(X,Y)	(map[NORM_X(X)][NORM_Y(Y)].occupied != (unsigned short)-1)


#define CALC_DIST(AX, AY, BX, BY) ((AX > BX ? AX - BX : BX - AX) % mhw) + ((AY > BY ? AY - BY : BY - AY) % mhh)


// Just some basic command macros
#define MOVE_STILL(S)		printf("m %hu o ", (S));
#define MOVE_NORTH(S)		printf("m %hu n ", (S));
#define MOVE_SOUTH(S)		printf("m %hu s ", (S));
#define MOVE_EAST(S)		printf("m %hu e ", (S));
#define MOVE_WEST(S)		printf("m %hu w ", (S));
#define CONVERT_DROPOFF(S)	printf("c %hu ", (S));
#define SPAWN_SHIP()		printf("g ");
