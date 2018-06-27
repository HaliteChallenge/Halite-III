import hlt
import random

game_map, players, myID = hlt.get_init()
hlt.send_init("MyPythonBot-"+str(myID))

while True:
	turn_number, players = hlt.get_frame()
	moves = { location: random.choice(["n", "e", "s", "w"]) for location in players[myID].entities.keys() }
	hlt.send_frame(moves)