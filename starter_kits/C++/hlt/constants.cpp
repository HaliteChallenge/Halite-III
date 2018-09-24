#include "constants.hpp"
#include "log.hpp"

#include <unordered_map>
#include <sstream>
#include <vector>

using namespace hlt;

namespace hlt {
    namespace constants {
        int MAX_HALITE;
        int SHIP_COST;
        int DROPOFF_COST;
        int MAX_TURNS;
        int EXTRACT_RATIO;
        int MOVE_COST_RATIO;
        bool INSPIRATION_ENABLED;
        int INSPIRATION_RADIUS;
        int INSPIRATION_SHIP_COUNT;
        int INSPIRED_EXTRACT_RATIO;
        double INSPIRED_BONUS_MULTIPLIER;
        int INSPIRED_MOVE_COST_RATIO;
    }
}

static std::string get_string(std::unordered_map<std::string, std::string>& map, const std::string& key) {
    auto it = map.find(key);
    if (it == map.end()) {
        log::log("Error: constants: server did not send " + key + " constant.");
        exit(1);
    }
    return it->second;
}

static int get_int(std::unordered_map<std::string, std::string>& map, const std::string& key) {
    return stoi(get_string(map, key));
}

static double get_double(std::unordered_map<std::string, std::string>& map, const std::string& key) {
    return stod(get_string(map, key));
}

static bool get_bool(std::unordered_map<std::string, std::string>& map, const std::string& key) {
    std::string string_value = get_string(map, key);
    if (string_value == "true") {
        return true;
    }
    if (string_value == "false") {
        return false;
    }

    log::log("Error: constants: " + key + " constant has value of '" + string_value +
        "' from server. Do not know how to parse that as boolean.");
    exit(1);
}

void hlt::constants::populate_constants(const std::string& string_from_engine) {
    std::string input;
    for (char c : string_from_engine) {
        switch (c) {
            case '{':
            case '}':
            case ',':
            case ':':
            case '"':
                input.push_back(' ');
                break;
            default:
                input.push_back(c);
                break;
        }
    }

    std::stringstream input_stream = std::stringstream(input);
    std::vector<std::string> tokens;

    for (;;) {
        std::string token;
        if (!std::getline(input_stream, token, ' ')) {
            break;
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    if ((tokens.size() % 2) != 0) {
        log::log("Error: constants: expected even total number of key and value tokens from server.");
        exit(1);
    }

    std::unordered_map<std::string, std::string> constants_map;

    for (size_t i = 0; i < tokens.size(); i += 2) {
        constants_map[tokens[i]] = tokens[i+1];
    }

    SHIP_COST = get_int(constants_map, "NEW_ENTITY_ENERGY_COST");
    DROPOFF_COST = get_int(constants_map, "DROPOFF_COST");
    MAX_HALITE = get_int(constants_map, "MAX_ENERGY");
    MAX_TURNS = get_int(constants_map, "MAX_TURNS");
    EXTRACT_RATIO = get_int(constants_map, "EXTRACT_RATIO");
    MOVE_COST_RATIO = get_int(constants_map, "MOVE_COST_RATIO");
    INSPIRATION_ENABLED = get_bool(constants_map, "INSPIRATION_ENABLED");
    INSPIRATION_RADIUS = get_int(constants_map, "INSPIRATION_RADIUS");
    INSPIRATION_SHIP_COUNT = get_int(constants_map, "INSPIRATION_SHIP_COUNT");
    INSPIRED_EXTRACT_RATIO = get_int(constants_map, "INSPIRED_EXTRACT_RATIO");
    INSPIRED_BONUS_MULTIPLIER = get_double(constants_map, "INSPIRED_BONUS_MULTIPLIER");
    INSPIRED_MOVE_COST_RATIO = get_int(constants_map, "INSPIRED_MOVE_COST_RATIO");
}
