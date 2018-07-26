#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

typedef long halite_type;
typedef long id_type;
typedef long dimension_type;

namespace hlt {
static constexpr halite_type MAX_HALITE = 1000;
static constexpr halite_type SHIP_COST = 500;
}

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
    bool operator!=(const Location & l) const { return x != l.x || y != l.y; }
};
static std::ostream & operator<<(std::ostream & ostream, const Location & l) {
    ostream << l.x << ' ' << l.y;
    return ostream;
}
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
    EAST = 'e',
    SOUTH = 's',
    WEST = 'w'
};
static std::ostream & operator<<(std::ostream & ostream, const Direction & direction) { return ostream << static_cast<char>(direction); }
static const std::array<Direction, 4> CARDINALS = { Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST };

struct Ship {
    id_type id;
    Location location;
    halite_type halite;
};
struct Dropoff {
    id_type id;
    Location location;
};

struct Player {
    id_type player_id;
    halite_type halite;
    Location shipyard;
    std::unordered_map<id_type, Ship> ships;
    std::unordered_map<id_type, Dropoff> dropoffs;
};
static std::istream & operator>>(std::istream & istream, Player & player) {
    long num_ships, num_dropoffs;
    istream >> player.player_id >> num_ships >> num_dropoffs >> player.halite;
    player.ships.clear();
    player.dropoffs.clear();
    for(int i = 0; i < num_ships; i++) {
        id_type id;
        Location l;
        halite_type h;
        istream >> id >> l >> h;
        player.ships[id] = { id, l, h };
    }
    for(int i = 0; i < num_dropoffs; i++) {
        id_type id;
        Location l;
        istream >> id >> l;
        player.dropoffs[id] = { id, l };
    }
    return istream;
}
typedef std::unordered_map<id_type, Player> Players;

struct Map {
    dimension_type width, height;
    std::vector< std::vector<halite_type> > grid;
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
    halite_type & operator[](const Location & l) {
        return grid[l.y][l.x];
    }
    const halite_type & operator[](const Location & l) const {
        return grid[l.y][l.x];
    }
    template<class T> std::vector<halite_type> & operator[](const T & y) {
        return grid[y];
    }
    template<class T> const std::vector<halite_type> & operator[](const T & y) const {
        return grid[y];
    }
};
static std::istream &operator>>(std::istream & istream, Map & map) {
    istream >> map.width >> map.height;
    map.grid = std::vector< std::vector<halite_type> >(map.height, std::vector<halite_type>(map.width));
    for (auto & row : map.grid) for (auto & cell : row) istream >> cell;
    return istream;
}

static void getInit(Map & map, Players & players, id_type & playerID) {
    int num_players;
    std::cin >> num_players >> playerID;
    for(int i = 0; i < num_players; i++) {
        id_type id;
        Location shipyard;
        std::cin >> id >> shipyard;
        players[id] = { id, 0, shipyard };
    }
    std::cin >> map;
}

static void sendInit(const std::string & name) {
    std::cout.sync_with_stdio(0);
    if(name.size() < 1) std::cout << ' ' << std::endl;
    else std::cout << name << std::endl;
}

static long getFrame(Map & map, Players & players) {
    long turn_number;
    std::cin >> turn_number;
    for(unsigned int i = 0; i < players.size(); i++) {
        Player p;
        std::cin >> p;
        p.shipyard = players[p.player_id].shipyard;
        players[p.player_id] = p;
    }
    long num_changed_cells;
    std::cin >> num_changed_cells;
    for(int i = 0; i < num_changed_cells; i++) {
        dimension_type x, y;
        halite_type halite;
        std::cin >> x >> y >> halite;
        map[y][x] = halite;
    }
    return turn_number;
}

namespace detail {
static std::ostringstream _moves;
}
static void move(const id_type & id, Direction d) {
    detail::_moves << "m " << id << ' ' << d << ' ';
}
static void spawn(const halite_type & halite) {
    detail::_moves << "g " << halite << ' ';
}
static void dump(const id_type & id, const halite_type & halite) {
    detail::_moves << "d " << id << ' ' << halite << ' ';
}
static void construct(const id_type & id) {
    detail::_moves << "c " << id << ' ';
}
static void sendFrame() {
    std::cout << detail::_moves.str() << std::endl;
    std::ostringstream().swap(detail::_moves);
}
}
