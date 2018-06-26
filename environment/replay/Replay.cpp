#include "Replay.hpp"
#include "Logging.hpp"

/** A JSON key and value corresponding to a field. */
#define TURN_FIELD_TO_JSON(x) {#x, turn.x}
#define REPLAY_FIELD_TO_JSON(x) {#x, replay->x}

namespace hlt {

/**
 * Convert turn information to JSON format.
 * @param[out] json The output JSON.
 * @param stats The turn to convert.
 */
void to_json(nlohmann::json &json, const Turn &turn) {
    json = {TURN_FIELD_TO_JSON(events)};
    nlohmann::json moves_json;
    for (auto &[player_id, commands] : turn.moves) {
        moves_json[std::to_string(player_id)] = commands;
    }
    json["moves"] = moves_json;
}

// Reserve space for constants to use in json functions
constexpr unsigned long Replay::REPLAY_FILE_VERSION;
constexpr char const *Replay::ENGINE_VERSION;

/**
 * Create json format for replay struct ptr
 * Ptr argument is useful for output replay function, which will pass the replay struct as "this"
 *
 * @param[out] json JSON to fill
 * @param replay: ptr to replay struct to convert to json
 */
void to_json(nlohmann::json &json, const Replay *replay) {
    json = {REPLAY_FIELD_TO_JSON(game_statistics),
            REPLAY_FIELD_TO_JSON(GAME_CONSTANTS),
            REPLAY_FIELD_TO_JSON(number_of_players),
            REPLAY_FIELD_TO_JSON(production_map),
            REPLAY_FIELD_TO_JSON(map_generator_seed),
            REPLAY_FIELD_TO_JSON(full_frames),
            REPLAY_FIELD_TO_JSON(REPLAY_FILE_VERSION),
            REPLAY_FIELD_TO_JSON(ENGINE_VERSION)};
    nlohmann::json players_json = nlohmann::json::array();
    for (auto &[_, player] : replay->players) {
        players_json.push_back(player);
    }
    json["players"] = players_json;
}

/**
 * Create json format for replay struct
 * Simply forwards to function requiring replay ptr.
 *
 * @param[out] json JSON to fill
 * @param replay Replay struct to convert to json
 */
void to_json(nlohmann::json &json, const Replay &replay) {
    nlohmann::to_json(json, &replay);
}

/**
 * Output replay into file. Replay will be in json format and may be compressed
 *
 * @param filename File to put replay into
 * @param enable_compression Switch to decide whether or not to compress replay file
 */
void Replay::output(std::string filename, bool enable_compression) {
    std::ofstream gameFile;
    gameFile.open(filename, std::ios_base::binary);
    if (!gameFile.is_open())
        throw std::runtime_error("Could not open file for replay");

    nlohmann::json replay_json;
    nlohmann::to_json(replay_json, this);

    std::string data = replay_json.dump();
    auto data_size = data.size();
    auto bin_data = reinterpret_cast<const unsigned char *>(data.data());

    // Use zstd to further compress replay file
    if (enable_compression) {
        auto compressed_length = ZSTD_compressBound(data_size);
        auto compressed_data = reinterpret_cast<uint8_t *>(std::malloc(compressed_length));
        auto result = ZSTD_compress(compressed_data, compressed_length,
                                    bin_data, data_size, ZSTD_maxCLevel());
        if (!ZSTD_isError(result)) {
            gameFile.write(reinterpret_cast<const char *>(compressed_data),
                           result * sizeof(uint8_t));
        } else {
            Logging::log("Warning: could not compress replay file! \n", Logging::Level::Warning);
            gameFile.write(reinterpret_cast<const char *>(data.data()), data_size);
        }

        std::free(compressed_data);
    } else {
        gameFile.write(reinterpret_cast<const char *>(data.data()), data_size);
    }

    gameFile.flush();
    gameFile.close();
}

}