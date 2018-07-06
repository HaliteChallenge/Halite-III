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

    for (unsigned long i = 0; i < num_players; i++) {
        Player::id_type player_id;
        energy_type energy;

        iss >> player_id;
        ignore_delimiter(iss, ';', 0);
        iss >> energy;
        ignore_delimiter(iss, ';', 0);

        players[player_id] = PlayerSnapshot{{}, energy, {}};

        // Parse factories
        while (iss && iss.peek() != ';') {
            dimension_type x, y;
            iss >> x;
            ignore_delimiter(iss, '-', 0);
            iss >> y;
            if (iss.peek() == ',') iss.ignore();

            players[player_id].factories.emplace_back(x, y);
        }
        ignore_delimiter(iss, ';', 0);

        // Parse entities
        while (iss && iss.peek() != ';' && iss.peek() != EOF) {
            dimension_type x, y;
            energy_type energy;
            iss >> x;
            ignore_delimiter(iss, '-', 0);
            iss >> y;
            ignore_delimiter(iss, '-', 0);
            iss >> energy;
            if (iss.peek() == ',') iss.ignore();

            players[player_id].entities.emplace_back(Location{x, y}, energy);
        }
        // Skip ; between players but don't require it at end
        if (iss) ignore_delimiter(iss, ';', 0);
    }

    return Snapshot{map_params, players};
}
}
