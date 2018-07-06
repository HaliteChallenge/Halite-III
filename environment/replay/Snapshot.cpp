#include "Snapshot.hpp"

#include <sstream>

#include "../version.hpp"

/**
 * Check the string stream and ignore the given delimiter character if
 * present. Otherwise, throw an exception. The string stream might be
 * a subset of the overall string stream, so use pos as an offset.
 */
void ignore_delimiter(std::istringstream& iss, char delim, int pos) {
    if (iss.peek() == delim) {
        iss.ignore();
    }
    else {
        std::stringstream msg;
        msg << "Expected "
            << delim
            << " but got "
            << (iss.peek() == EOF ? "EOF" : std::string(1, iss.peek()));
        throw SnapshotError(msg.str(), pos + iss.tellg());
    }
}

namespace hlt {

Snapshot Snapshot::from_str(const std::string& snapshot) {
    std::istringstream iss{snapshot};
    std::string buf;

    if (!std::getline(iss, buf, ';')) {
        throw SnapshotError(
            "EOF while parsing engine version from snapshot",
            snapshot.size()
        );
    }
    if (buf != HALITE_VERSION) {
        throw SnapshotError(
            "Halite engine version does not match",
            iss.tellg()
        );
    }

    if (!std::getline(iss, buf, ';')) {
        throw SnapshotError(
            "EOF while parsing mapgen parameters from snapshot",
            snapshot.size()
        );
    }
    std::string mapbuf;
    std::istringstream mapiss{buf};
    if (!std::getline(mapiss, mapbuf, ',')) {
        throw SnapshotError(
            "EOF while parsing mapgen algorithm",
            snapshot.size()
        );
    }
    // TODO: parse all mapgen algorithms
    if (mapbuf != "Fractal Value Noise Tile") {
        throw SnapshotError(
            "Unrecognized mapgen algorithm",
            iss.tellg()
        );
    }
    dimension_type width;
    dimension_type height;
    unsigned long num_players;
    unsigned int seed;

    mapiss >> width;
    ignore_delimiter(mapiss, ',', iss.tellg());
    mapiss >> height;
    ignore_delimiter(mapiss, ',', iss.tellg());
    mapiss >> num_players;
    ignore_delimiter(mapiss, ',', iss.tellg());
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
        throw SnapshotError(
            "EOF while parsing players from snapshot",
            snapshot.size()
        );
    }
    std::string playerbuf;
    std::istringstream playeriss{buf};

    for (unsigned long i = 0; i < num_players; i++) {
        if (!std::getline(playeriss, playerbuf, ',')) {
            throw SnapshotError(
                "EOF while parsing players from snapshot",
                snapshot.size()
            );
        }

        Player::id_type player_id;
        energy_type energy;
        dimension_type x;
        dimension_type y;

        std::istringstream player{playerbuf};
        player >> player_id;
        ignore_delimiter(player, '-', iss.tellg());
        player >> x;
        ignore_delimiter(player, '-', iss.tellg());
        player >> y;
        ignore_delimiter(player, '-', iss.tellg());
        player >> energy;

        players[player_id] = PlayerSnapshot{Location{x,y}, energy, {}};
    }

    // Parse player entities
    while (iss) {
        Player::id_type player_id;
        iss >> player_id;
        ignore_delimiter(iss, ';', 0);

        if (!std::getline(iss, buf, ';')) {
            throw SnapshotError(
                "EOF while parsing players from snapshot",
                snapshot.size()
            );
        }

        if (buf.size() == 0) {
            // Player has no entities
            continue;
        }

        std::istringstream entityiss{buf};
        while (entityiss) {
            dimension_type x;
            dimension_type y;
            energy_type energy;

            entityiss >> x;
            ignore_delimiter(entityiss, '-', iss.tellg());
            entityiss >> y;
            ignore_delimiter(entityiss, '-', iss.tellg());

            entityiss >> energy;
            // Ignore comma if present, but don't require it
            if (entityiss.peek() == ',') entityiss.ignore();

            players[player_id].entities.emplace_back(std::make_pair(Location{x, y}, energy));
        }
    }

    return Snapshot{map_params, players};
}
}
