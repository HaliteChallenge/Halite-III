// Let's do some really stupid and easy move by default
void default_move(unsigned short ship_id){
	unsigned char decision = random() % 100;

	// 10% chance each to move north/east/south/west 
	if(decision <= 10)
		printf("m %hu n ", ship_id);
	else if(decision <= 20)
		printf("m %hu e ", ship_id);
	else if(decision <= 30)
		printf("m %hu s ", ship_id);
	else if(decision <= 40)
		printf("m %hu w ", ship_id);
	else{
		// And a 60% chance to stay still
		printf("m %hu o ", ship_id);
		// We could alternatively just output nothing
	}
}