#include "Snapshot.hpp"

#include <sstream>

#include "../version.hpp"

std::pair<hlt::Snapshot, std::string> error(std::string msg) {
    return std::make_pair(hlt::Snapshot{}, msg);
}

namespace hlt {

std::pair<Snapshot, std::string> Snapshot::from_str(const std::string& snapshot) {
    std::istringstream iss{snapshot};
    std::string buf;

    if (!std::getline(iss, buf, ';')) {
        return error("EOF while parsing engine version from snapshot");
    }
    if (buf != HALITE_VERSION) {
        return error("Halite engine version does not match");
    }

    if (!std::getline(iss, buf, ';')) {
        return error("EOF while parsing mapgen parameters from snapshot");
    }
    std::string mapbuf;
    std::istringstream mapiss{buf};
    if (!std::getline(mapiss, mapbuf, ',')) {
        return error("EOF while parsing mapgen algorithm");
    }
    // TODO: parse all mapgen algorithms
    if (mapbuf != "Fractal Value Noise Tile") {
        return error("Unrecognized mapgen algorithm");
    }
    dimension_type width;
    dimension_type height;
    unsigned long num_players;
    unsigned int seed;

    mapiss >> width;
    if (mapiss.peek() == ',') mapiss.ignore();
    else return error("Could not parse mapgen parameters (expected comma)");
    mapiss >> height;
    if (mapiss.peek() == ',') mapiss.ignore();
    else return error("Could not parse mapgen parameters (expected comma)");
    mapiss >> num_players;
    if (mapiss.peek() == ',') mapiss.ignore();
    else return error("Could not parse mapgen parameters (expected comma)");
    mapiss >> seed;

    auto map_params = mapgen::MapParameters{
        mapgen::MapType::Fractal,
        seed,
        width,
        height,
        num_players,
    };

    std::unordered_map<Player::id_type, PlayerSnapshot> players;

    // Parse factories and player locations
    if (!std::getline(iss, buf, ';')) {
        return error("EOF while parsing players from snapshot");
    }
    std::string playerbuf;
    std::istringstream playeriss{buf};

    for (unsigned long i = 0; i < num_players; i++) {
        if (!std::getline(playeriss, playerbuf, ',')) {
            return error("EOF while parsing players from snapshot");
        }

        Player::id_type player_id;
        // TODO: parse energy from snapshot
        energy_type energy = 20000;
        dimension_type x;
        dimension_type y;

        std::istringstream player{playerbuf};
        player >> player_id;
        if (player.peek() == '-') player.ignore();
        else return error("Could not parse player (expected dash)");
        player >> x;
        if (player.peek() == '-') player.ignore();
        else return error("Could not parse player (expected dash)");
        player >> y;

        players[player_id] = PlayerSnapshot{Location{x,y}, energy, {}};
    }

    // Parse player entities
    for (unsigned long i = 0; i < num_players; i++) {
        Player::id_type player_id;
        iss >> player_id;
        if (iss.peek() == ';') iss.ignore();
        else return error("Could not parse player (expected semicolon)");

        if (!std::getline(iss, buf, ';')) {
            return error("EOF while parsing players from snapshot");
        }

        std::istringstream entityiss{buf};
        while (entityiss) {
            dimension_type x;
            dimension_type y;
            energy_type energy;

            entityiss >> x;
            if (entityiss.peek() == '-') entityiss.ignore();
            else return error("Could not parse player entity (expected dash)");

            entityiss >> y;
            if (entityiss.peek() == '-') entityiss.ignore();
            else return error("Could not parse player entity (expected dash)");

            entityiss >> energy;
            // Ignore comma if present, but don't require it
            if (entityiss.peek() == ',') entityiss.ignore();

            players[player_id].entities.emplace_back(std::make_pair(Location{x, y}, energy));
        }
    }

    return std::make_pair(Snapshot{map_params, players}, "");
}
}
