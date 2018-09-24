#ifndef DROPOFF_HPP
#define DROPOFF_HPP

#include "Enumerated.hpp"
#include "Location.hpp"

namespace hlt {

/** A dropoff owned by a player. */
struct Dropoff final : Enumerated<Dropoff> {
    /** Location of the dropoff. */
    const Location location;

    /** How much halite has been deposited here so far. */
    energy_type deposited_halite{};

    /**
     * Output the dropoff to a stream.
     * @param ostream The output stream.
     * @param dropoff The dropoff.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &ostream, const Dropoff &dropoff) {
        return ostream << dropoff.id << " " << dropoff.location;
    }

    /**
     * Construct Dropoff from ID and location.
     * @param id The ID.
     * @param location The location.
     */
    explicit Dropoff(id_type id, Location location) : Enumerated(id), location(location) {}
};

}

#endif // DROPOFF_HPP
