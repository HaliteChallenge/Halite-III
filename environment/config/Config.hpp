#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace hlt {

/**
 * Global configuration settings that pertain not to gameplay but rather to engine operation.
 * May be modified by command-line arguments.
 */
struct Config {
    long game_id{};                 /**< The game ID. */
    unsigned int seed{};            /**< The random number generation seed. */
    bool enable_replay = true;      /**< Enable the replay feature. */
    bool enable_compression = true; /**< Enable compression. */
};

}

#endif // CONFIG_HPP
