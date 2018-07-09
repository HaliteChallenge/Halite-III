#ifndef HALITE_REPLAY_HPP
#define HALITE_REPLAY_HPP

#include <fstream>
#include <iostream>
#include <string>

#include "nlohmann/json.hpp"
#include "zstd.h"

#include "Statistics.hpp"
#include "Command.hpp"
#include "GameEvent.hpp"
#include "../version.hpp"
#include "Constants.hpp"

namespace hlt {

struct Turn {
    std::unordered_map<Player::id_type, std::vector<Command>> moves;    /**< Mapping from player id to the commands they issued this turn */
    std::unordered_map<Player::id_type, energy_type> energy;            /**< Mapping from player id to the energy they ended the turn with */
    std::vector<GameEvent> events;                                      /**< Events occurring this turn (spawns, deaths, etc) for replay */

    /**
     * Convert turn to JSON format.
     * @param[out] json The output JSON.
     * @param stats The turn to convert.
     */
    friend void to_json(nlohmann::json &json, const Turn &turn);
};

struct Replay {
    GameStatistics &game_statistics;                            /**< Statistics for the game (inlcudes number of turns) */
    const Constants &GAME_CONSTANTS = Constants{}.get();        /**< Constants used in this game */
    static constexpr unsigned long REPLAY_FILE_VERSION = 2;     /**< Replay file version (updated as this struct or serialization changes) */
    static constexpr auto ENGINE_VERSION = HALITE_VERSION;      /**< Version of the game engine */

    size_t number_of_players;                                   /**< Number of players in this game */
    std::unordered_map<Player::id_type, Player> players{};                                  /**< List of players at start of game, including factory location and initial entities */
    std::vector<hlt::Turn> full_frames{};                       /**< Turn information: first element = first frame/turn. Length is game_statistics.number_turns */

    unsigned int map_generator_seed;                            /**< Seed used in random number generator for map */
    Map &production_map;                                        /**< Map of cells game was played on, including factory and other cells. Struct incldues name of map generator */

    /**
     * Create json format for replay struct ptr
     * Ptr argument is useful for output replay function, which will pass the replay struct as "this"
     *
     * @param[out] json JSON to fill
     * @param replay: ptr to replay struct to convert to json
     */
    friend void to_json(nlohmann::json &json, const Replay *replay);

    /**
     * Create json format for replay struct
     * Simply forwards to function requiring replay ptr.
     *
     * @param[out] json JSON to fill
     * @param replay Replay struct to convert to json
     */
    friend void to_json(nlohmann::json &json, const Replay &replay);

    /**
     * Output replay into file. Replay will be in json format and may be compressed
     *
     * @param filename File to put replay into
     * @param enable_compression Switch to decide whether or not to compress replay file
     */
    void output(std::string filename, bool enable_compression);

    /** Create Replay from Game statistics, number of players, initial list of players, seed for the map generation, and the generated map
     *
     * @param game_statistics Reference access to game_statistics struct that will be updated during game play
     * @param number_of_players Number of players in this game
     * @param players Initial player list -> should be initialized with factory location and any starting entities
     * @param seed Seed for random number generator for map
     * @param production_map Initialized map for game play
     */
    Replay(GameStatistics &game_statistics, size_t number_of_players, unsigned int seed, Map &production_map) :
            game_statistics(game_statistics), number_of_players(number_of_players),
            map_generator_seed(seed), production_map(production_map) {}

    /**
     * Default destructor for class
     */
    ~Replay() = default;
};

}

#endif //HALITE_REPLAY_HPP
