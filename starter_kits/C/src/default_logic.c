// Let's do some movement that's really easy and "smart"!
void random_move(unsigned short ship_id){
	unsigned char decision = rand() % 100;

	// 10% chance each to move north/east/south/west (This is extra smart)
	if(decision <= 10){
		MOVE_NORTH(ship_id);
	}else if(decision <= 20){
		MOVE_EAST(ship_id);
	}else if(decision <= 30){
		MOVE_SOUTH(ship_id);
	}else if(decision <= 40){
		MOVE_WEST(ship_id);
	}else{
		// And a 60% chance to stay still (This is also extra smart)
		// We could alternatively just not give the ship a move this turn
		MOVE_STILL(ship_id);
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
	if(decision <= 10 && TILE_OCCUPIED(x, y-1) == 0){
		MOVE_NORTH(ship_id);
	}else if(decision <= 20 && TILE_OCCUPIED(x+1, y) == 0){
		MOVE_EAST(ship_id);
	}else if(decision <= 30 && TILE_OCCUPIED(x, y+1) == 0){
		MOVE_SOUTH(ship_id);
	}else if(decision <= 40 && TILE_OCCUPIED(x-1, y) == 0){
		MOVE_WEST(ship_id);
	}else{
		// And a 60% chance to stay still (This is also smarter than before!)
		MOVE_STILL(ship_id);
	}
}
