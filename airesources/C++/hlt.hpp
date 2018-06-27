#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <exception>
#include <unordered_map>
#include <fstream>

typedef long energy_type;
typedef long id_type;
typedef long dimension_type;

namespace hlt {
struct Location {
	dimension_type x, y;
	bool operator==(const Location & l) const { return x == l.x && y == l.y; }
};
static std::istream & operator>>(std::istream & istream, Location & l) {
	istream >> l.x >> l.y;
	return istream;
}
}
namespace std {
template <>
struct hash<hlt::Location> {
	std::size_t operator()(const hlt::Location & l) const {
		return ((l.x+l.y) * (l.x+l.y+1) / 2) + l.y;
	}
};
}

namespace hlt {

enum class Direction : char {
    NORTH = 'n',
    SOUTH = 's',
    EAST = 'e',
    WEST = 'w',
};
/* Probably not going to use this; useful for random though
static Direction directionFromInt(int d) {
	switch(d) {
		// No present value for still -- API is to give no command
		case 1: return 'n';
		case 2: return 'e';
		case 3: return 's';
		case 4: return 'w';
	}
} */
typedef std::unordered_map<Location, Direction> Moves;
static std::ostream & operator<<(std::ostream & ostream, const Moves & moves) {
	for(auto &[location, direction] : moves) {
		ostream << "m " << location.x << ' ' << location.y << ' ' << static_cast<char>(direction) << ' ';
	}
	return ostream;
}

struct Entity {
    id_type owner_id; /**< ID of the owner. */
    energy_type energy;       /**< Energy of the entity. */
};
static std::istream & operator>>(std::istream & istream, Entity & entity) {
    istream >> entity.owner_id >> entity.energy;
    istream.get(); // Take up the newline
    return istream;
}
typedef std::unordered_map<Location, hlt::Entity> Entities;

struct Player {
	id_type player_id;
	energy_type energy;
	Location factory_location;
	Entities entities;
	// Adds accounting for entity merging. Prefer this when modifying the state.
	void add_entity(const Location & location, const Entity & entity) {
		auto entity_iterator = entities.find(location);
		// If the player already has an entity there, merge
	    if (entity_iterator != entities.end()) entity_iterator->second.energy += entity.energy;
	    // Otherwise, move this entity there
	    else entities[location] = entity;
	}
};
static std::istream & operator>>(std::istream & istream, Player & player) {
	long num_entities;
    istream >> player.player_id >> num_entities >> player.energy;
    player.entities.clear();
    for(int i = 0; i < num_entities; i++) {
    	Location l;
    	Entity e;
    	istream >> l >> e;
    	player.entities[l] = e;
    }
    return istream;
}
typedef std::unordered_map<int, Player> Players;

struct Cell {
	long production;
	bool passable;
	long energy_factor;
};
static std::istream &operator>>(std::istream & istream, Cell & cell) {
	// Reset, then read
	cell = { 0, true, 0 };
	char c;
	do {
		c = istream.get();
	} while(c != 'n' && c != 'o' && c != 'e' && c != 'f');
	if(c == 'n') istream >> cell.production;
	else if(c == 'o') {
		istream >> cell.production;
		cell.passable = false;
	}
	else if(c == 'e') istream >> cell.production >> cell.energy_factor;
	else if(c == 'f'); // Do nothing
	else throw std::runtime_error("https://i.imgflip.com/28gy3a.jpg");
	return istream;
}

struct Map {
	dimension_type width, height;
	std::vector< std::vector<Cell> > grid;
};
static std::istream &operator>>(std::istream & istream, Map & map) {
	istream >> map.width >> map.height;
	map.grid = std::vector< std::vector<Cell> >(map.height, std::vector<Cell>(map.width));
	for (auto & row : map.grid) for (auto & cell : row) istream >> cell;
	return istream;
}

static void getInit(Map & map, Players & players, id_type & playerID) {
	int num_players;
	std::cin >> num_players >> playerID;
	for(int i = 0; i < num_players; i++) {
		id_type id;
		Location factory_location;
		std::cin >> id >> factory_location;
		players[id] = { id, 0, factory_location, Entities{} };
	}
	std::cin >> map;
}

static void sendInit(const std::string & name) {
	if(name.size() < 1) std::cout << ' ' << std::endl;
	else std::cout << name << std::endl;
}

static void getFrame(Players & players) {
	long _;
	std::cin >> _; // Don't care about turn number yet.
	for(int i = 0; i < players.size(); i++) {
		Player p;
		std::cin >> p;
		players[p.player_id] = p;
	}
}

static void sendFrame(const Moves & moves) {
	std::cout << moves << std::flush;
}

}

struct Log {
private:
    std::ofstream file;

    void initialize(const std::string& filename) {
        file.open(filename, std::ios::trunc | std::ios::out);
    }

public:
    static Log& get() {
        static Log instance{};
        return instance;
    }

    static void open(const std::string& filename) {
        get().initialize(filename);
    }

    static void log(const std::string& message) {
        get().file << message << std::endl;
    }
};