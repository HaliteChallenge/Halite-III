#ifndef HALITEIMPL_HPP
#define HALITEIMPL_HPP

#include "Halite.hpp"

namespace hlt {

/** Halite implementation class, expressing core game logic. */
class HaliteImpl final {
    friend class Halite;

    /** Pointer to the game interface. Raw because HaliteImpl is always owned by Halite. */
    Halite *game;

    /** Communicate with bots to obtain moves for next step. */
    void retrieve_moves();

    /** Process all commands for this turn. */
    void process_commands();

    /** Process all entity lifecycle events for this turn. */
    void process_entities();

public:
    /**
     * Construct HaliteImpl from game interface.
     * @param game The game interface.
     */
    explicit HaliteImpl(Halite *game) : game(game) {}
};

}

#endif // HALITEIMPL_HPP
