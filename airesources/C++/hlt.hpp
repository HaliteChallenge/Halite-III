#pragma once

#include <string>
#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <exception>
#include <fstream>

typedef long energy_type;
typedef long id_type;
typedef long dimension_type;

struct Logging {
private:
    std::ofstream file;

    void initialize(const std::string& filename) {
        file.open(filename, std::ios::trunc | std::ios::out);
    }

public:
    static Logging & get() {
        static Logging instance{};
        return instance;
    }

    static void open(const std::string& filename) {
        get().initialize(filename);
    }

    static void log(const std::string& message) {
        get().file << message << std::endl;
    }
};

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
    STILL = 'o',
    NORTH = 'n',
    EAST = 'e',
    SOUTH = 's',
    WEST = 'w'
};
static const std::array<Direction, 5> DIRECTIONS = { Direction::STILL, Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST };
static const std::array<Direction, 4> CARDINALS = { Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST };
typedef std::unordered_map<Location, Direction> Moves;
static std::ostream & operator<<(std::ostream & ostream, const Moves & moves) {
	for(auto &[location, direction] : moves) {
		if(direction != Direction::STILL) {
			ostream << "m " << location.x << ' ' << location.y << ' ' << static_cast<char>(direction) << ' ';
		}
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
	dimension_type distance(const Location & l1, const Location & l2) const {
	    const dimension_type x_dist = std::abs(l2.x - l1.x);
	    const dimension_type y_dist = std::abs(l2.y - l1.y);
	    return std::min(x_dist, width - x_dist) + std::min(y_dist, height - y_dist);
	}
	Location getLocation(Location l, const hlt::Direction & d) const {
	    if(d == Direction::NORTH) {
	    	l.y--;
	    	if(l.y == -1) l.y = height-1;
	    }
	    else if(d == Direction::EAST) {
	    	l.x++;
	    	if(l.x == width) l.x = 0;
	    }
	    else if(d == Direction::SOUTH) {
	    	l.y++;
	    	if(l.y == height) l.y = 0;
	    }
	    else if(d == Direction::WEST) {
	    	l.x--;
	    	if(l.x == -1) l.x = width-1;
	    }
	    return l;
	}
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

static long getFrame(Players & players) {
	long turn_number;
	std::cin >> turn_number;
	for(unsigned int i = 0; i < players.size(); i++) {
		Player p;
		std::cin >> p;
		p.factory_location = players[p.player_id].factory_location;
		players[p.player_id] = p;
	}
	return turn_number;
}

static void sendFrame(const Moves & moves) {
	std::cout << moves << std::endl;
}
}