#include "Snapshot.hpp"

#include "../version.hpp"

/**
 * Check the string stream and ignore the given delimiter character if
 * present. Otherwise, throw an exception. The string stream might be
 * a subset of the overall string stream, so use pos as an offset.
 */
void ignore_delimiter(std::istringstream &iss, char delim, int pos) {
    if (iss.peek() == delim) {
        iss.ignore();
    } else {
        std::stringstream msg;
        msg << "Expected "
            << delim
            << " but got "
            << (iss.peek() == EOF ? "EOF" : std::string(1, iss.peek()));
        throw SnapshotError(msg.str(), pos + iss.tellg());
    }
}

namespace hlt {

Snapshot Snapshot::from_str(const std::string &snapshot) {
    std::istringstream iss{snapshot};
    std::string buf;

    if (!std::getline(iss, buf, SNAPSHOT_FIELD_DELIMITER)) {
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

    hlt::mapgen::MapType map_generator;
    dimension_type width;
    dimension_type height;
    unsigned long num_players;
    unsigned int seed;

    std::getline(iss, buf, SNAPSHOT_LIST_DELIMITER);
    std::istringstream maptype{buf};
    maptype >> map_generator;
    iss >> width;
    ignore_delimiter(iss, SNAPSHOT_LIST_DELIMITER, iss.tellg());
    iss >> height;
    ignore_delimiter(iss, SNAPSHOT_LIST_DELIMITER, iss.tellg());
    iss >> num_players;
    ignore_delimiter(iss, SNAPSHOT_LIST_DELIMITER, iss.tellg());
    iss >> seed;
    ignore_delimiter(iss, SNAPSHOT_FIELD_DELIMITER, iss.tellg());

    auto map_params = mapgen::MapParameters{
            mapgen::MapType::Fractal,
            seed,
            width,
            height,
            num_players,
    };

    // Parse map
    std::vector<energy_type> map;
    for (auto i = 0; i < width * height; i++) {
        energy_type energy;
        iss >> energy;
        map.push_back(energy);
        if (iss.peek() == SNAPSHOT_LIST_DELIMITER) iss.ignore();
    }
    ignore_delimiter(iss, SNAPSHOT_FIELD_DELIMITER, 0);

    std::unordered_map<Player::id_type, PlayerSnapshot> players;
    for (unsigned long i = 0; i < num_players; i++) {
        Player::id_type player_id;
        energy_type energy;

        iss >> player_id;
        ignore_delimiter(iss, SNAPSHOT_FIELD_DELIMITER, 0);
        iss >> energy;
        ignore_delimiter(iss, SNAPSHOT_FIELD_DELIMITER, 0);

        players[player_id] = PlayerSnapshot{energy, {0, 0}, {}, {}};

        // Parse shipyard
        {
            dimension_type x, y;
            iss >> x;
            ignore_delimiter(iss, SNAPSHOT_SUBFIELD_DELIMITER, 0);
            iss >> y;
            if (iss.peek() == SNAPSHOT_LIST_DELIMITER) iss.ignore();
            players[player_id].factory = Location{x, y};
        }

        // Parse dropoffs
        while (iss && iss.peek() != SNAPSHOT_FIELD_DELIMITER) {
            Dropoff::id_type dropoff_id;
            dimension_type x, y;
            iss >> dropoff_id;
            ignore_delimiter(iss, SNAPSHOT_SUBFIELD_DELIMITER, 0);
            iss >> x;
            ignore_delimiter(iss, SNAPSHOT_SUBFIELD_DELIMITER, 0);
            iss >> y;
            if (iss.peek() == SNAPSHOT_LIST_DELIMITER) iss.ignore();

            players[player_id].dropoffs.emplace_back(dropoff_id, Location{x, y});
        }
        ignore_delimiter(iss, SNAPSHOT_FIELD_DELIMITER, 0);

        // Parse entities
        while (iss && iss.peek() != SNAPSHOT_FIELD_DELIMITER && iss.peek() != EOF) {
            Entity::id_type entity_id;
            dimension_type x, y;
            energy_type energy;
            iss >> entity_id;
            ignore_delimiter(iss, SNAPSHOT_SUBFIELD_DELIMITER, 0);
            iss >> x;
            ignore_delimiter(iss, SNAPSHOT_SUBFIELD_DELIMITER, 0);
            iss >> y;
            ignore_delimiter(iss, SNAPSHOT_SUBFIELD_DELIMITER, 0);
            iss >> energy;
            if (iss.peek() == SNAPSHOT_LIST_DELIMITER) iss.ignore();

            players[player_id].entities.emplace_back(entity_id, energy, Location{x, y});
        }
        // Skip ; between players but don't require it at end
        if (iss) ignore_delimiter(iss, SNAPSHOT_FIELD_DELIMITER, 0);
    }

    return Snapshot{map_params, map, players};
}
}
