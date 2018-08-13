#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <random>

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
    enum class Direction : char {
        NORTH = 'n',
        EAST = 'e',
        SOUTH = 's',
        WEST = 'w',
        STILL = 'o',
    };

    static std::ostream & operator<<(std::ostream & ostream, const Direction & direction) {
        return ostream << static_cast<char>(direction);
    }

    static const std::array<Direction, 4> CARDINALS = {
        { Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST }
    };

    struct Location {
        dimension_type x, y;
        bool operator==(const Location & l) const { return x == l.x && y == l.y; }
        bool operator!=(const Location & l) const { return x != l.x || y != l.y; }

        Location with_direction(Direction d) const {
            auto dx = 0;
            auto dy = 0;
            switch (d) {
            case Direction::NORTH:
                dy = -1;
                break;
            case Direction::SOUTH:
                dy = 1;
                break;
            case Direction::EAST:
                dx = 1;
                break;
            case Direction::WEST:
                dx = -1;
                break;
            case Direction::STILL:
                // No move
                break;
            }
            return Location{x + dx, y + dy};
        }
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
    struct Command {
        virtual void send() const = 0;
    };

    struct Nothing : Command {
        void send() const override {}
    };

    struct Spawn : Command {
        halite_type initial_fuel;

        Spawn(halite_type initial_fuel) : initial_fuel{initial_fuel} {}

        void send() const override {
            std::cout << "g " << initial_fuel;
        }
    };

    struct Move : Command {
        id_type id;
        Direction direction;

        Move(id_type id, Direction direction) : id{id}, direction{direction} {}

        void send() const override {
            std::cout << "m " << id << " " << direction;
        }
    };

    struct Dump : Command {
        id_type id;
        halite_type amount;

        Dump(id_type id, halite_type amount) : id{id}, amount{amount} {}

        void send() const override {
            std::cout << "d " << id << " " << amount;
        }
    };

    struct Construct : Command {
        id_type id;

        Construct(id_type id) : id{id} {}

        void send() const override {
            std::cout << "c " << id;
        }
    };

    struct Ship {
        id_type id;
        Location location;
        halite_type halite;

        bool is_full() const { return halite >= MAX_HALITE; }

        std::unique_ptr<Command> move_unsafe(Direction d) const { return std::make_unique<Move>(id, d); }
        std::unique_ptr<Command> dump(halite_type amount) const { return std::make_unique<Dump>(id, amount); }
        std::unique_ptr<Command> construct() const { return std::make_unique<Construct>(id); }
    };

    struct Dropoff {
        id_type id;
        Location location;

        Dropoff() = default;
        Dropoff(id_type id, Location location) : id{id}, location{location} {}
    };

    struct Shipyard : Dropoff {
        Shipyard() : Dropoff() {}
        Shipyard(Location location) : Dropoff(0, location) {}

        std::unique_ptr<Command> spawn(halite_type initial_fuel) {
            return std::make_unique<Spawn>(initial_fuel);
        }
    };

    struct Player {
        id_type player_id;
        halite_type halite;
        Shipyard shipyard;
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

        void normalize(Location location) const {
            location.x = (location.x + width) % width;
            location.y = (location.y + height) % height;
        }

        std::pair<Direction, bool> towards(Location start, Location target) const {
            if (start.x != target.x) {
                auto east_dist = 0;
                auto west_dist = 0;
                if (start.x > target.x) {
                    west_dist = start.x - target.x;
                    east_dist = width - west_dist;
                }
                else {
                    east_dist = target.x - start.x;
                    west_dist = width - east_dist;
                }

                if (west_dist < east_dist) {
                    return std::make_pair(Direction::WEST, true);
                }
                return std::make_pair(Direction::EAST, true);
            }
            else if (start.y != target.y) {
                auto north_dist = 0;
                auto south_dist = 0;

                if (start.y > target.y) {
                    north_dist = start.y - target.y;
                    south_dist = height - north_dist;
                }
                else {
                    south_dist = start.y - target.y;
                    north_dist = height - south_dist;
                }

                if (north_dist < south_dist) {
                    return std::make_pair(Direction::NORTH, true);
                }
                return std::make_pair(Direction::SOUTH, true);
            }
            else {
                return std::make_pair(Direction::NORTH, false);
            }
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

    struct Game {
        Map game_map;
        std::vector<Player> players;
        id_type my_id;

        void get_init();
        void send_init(const std::string& name);
        long get_frame();
        void end_turn(const std::vector<std::unique_ptr<Command>>& commands);

        Player& me();
    };

    struct SafeMover {
        const Map& game_map;
        const std::vector<Player>& players;
        std::vector<Location> moves;
        std::mt19937 prg;

        SafeMover(const Map& game_map, const std::vector<Player>& players) :
            game_map(game_map), players(players), prg(time(NULL)) {}

        std::unique_ptr<Command> move(const Ship& ship, Direction direction,
                                      unsigned int tries = 5) {
            if (tries == 0) {
                return std::make_unique<Nothing>();
            }

            auto target = ship.location.with_direction(direction);
            game_map.normalize(target);

            for (const auto& other_target : moves) {
                if (target == other_target) {
                    return move(ship, CARDINALS[prg() % 4], tries - 1);
                }
            }

            for (const auto& player : players) {
                for (const auto& ship_pair : player.ships) {
                    if (ship_pair.second.location == target) {
                        return move(ship, CARDINALS[prg() % 4], tries - 1);
                    }
                }
            }

            moves.push_back(target);

            return ship.move_unsafe(direction);
        }

        std::unique_ptr<Command> move_towards(const Ship& ship, const Location& target) {
            const auto towards = game_map.towards(ship.location, target);
            if (towards.second) {
                return move(ship, towards.first);
            }
            return std::unique_ptr<Nothing>();
        }
    };

    static std::istream &operator>>(std::istream & istream, Map & map) {
        istream >> map.width >> map.height;
        map.grid = std::vector< std::vector<halite_type> >(map.height, std::vector<halite_type>(map.width));
        for (auto & row : map.grid) for (auto & cell : row) istream >> cell;
        return istream;
    }

    Player& Game::me() {
        return players[my_id];
    }

    void Game::get_init() {
        int num_players;

        std::cin >> num_players >> my_id;
        players = std::vector<Player>(num_players);

        for (int i = 0; i < num_players; i++) {
            id_type id;
            Location shipyard;
            std::cin >> id >> shipyard;
            players[id] = { id, 0, {shipyard}, {}, {} };
        }

        std::cin >> game_map;
    }

    void Game::send_init(const std::string& name) {
        std::cout.sync_with_stdio(0);
        std::cout << name << std::endl;
    }

    long Game::get_frame() {
        long turn_number;
        std::cin >> turn_number;

        for (unsigned int i = 0; i < players.size(); i++) {
            std::cin >> players[i];
        }

        long num_changed_cells;
        std::cin >> num_changed_cells;
        for(int i = 0; i < num_changed_cells; i++) {
            dimension_type x, y;
            halite_type halite;
            std::cin >> x >> y >> halite;
            game_map[y][x] = halite;
        }
        return turn_number;
    }

    void Game::end_turn(const std::vector<std::unique_ptr<Command>>& commands) {
        for (const auto& command : commands) {
            command->send();
            std::cout << ' ';
        }
        std::cout << std::endl;
    }
}
