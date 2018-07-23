#ifndef HALITE_SNAPSHOT_HPP
#define HALITE_SNAPSHOT_HPP

#include <string>
#include <unordered_map>
#include <utility>
#include "Player.hpp"
#include "SnapshotError.hpp"
#include "Generator.hpp"

namespace hlt {

constexpr auto SNAPSHOT_FIELD_DELIMITER = ';';
constexpr auto SNAPSHOT_LIST_DELIMITER = ',';
constexpr auto SNAPSHOT_SUBFIELD_DELIMITER = '-';

struct PlayerSnapshot {
    energy_type energy;
    Location factory{0,0};
    std::vector<std::pair<Dropoff::id_type, Location>> dropoffs;
    std::vector<std::pair<Location, energy_type>> entities;

    PlayerSnapshot() = default;
    ~PlayerSnapshot() = default;
};

struct Snapshot {
    mapgen::MapParameters map_param{};
    std::unordered_map<Player::id_type, PlayerSnapshot> players;

    static Snapshot from_str(const std::string &snapshot);

    /**
     * Create snapshot from parameters
     */
    Snapshot(mapgen::MapParameters map_param,
             std::unordered_map<Player::id_type, PlayerSnapshot> players) :
            map_param{map_param}, players{std::move(players)} {}

    Snapshot() = default;

    /**
     * Default destructor for class
     */
    ~Snapshot() = default;
};

}

#endif //HALITE_REPLAY_HPP
