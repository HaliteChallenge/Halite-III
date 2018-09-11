#include "constants.hpp"
#include "log.hpp"

#include "jsmn/jsmn.h"

#include <iostream>
#include <cstring>
#include <unordered_map>

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
        bool CAPTURE_ENABLED;
        int CAPTURE_RADIUS;
        int CAPTURE_SHIP_ADVANTAGE;
    }
}

constexpr static unsigned int json_max_tokens = 128;

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

void hlt::constants::populate_constants(std::string string_from_engine) {
    const char* input = string_from_engine.c_str();
    size_t length = string_from_engine.length();

    jsmn_parser parser;
    jsmn_init(&parser);

    jsmntok_t tokens[json_max_tokens];
    int result = jsmn_parse(&parser, input, length, tokens, json_max_tokens);
    if (result < 0) {
        log::log("Error: constants: json parser returned " + std::to_string(result));
        exit(1);
    }

    if (result < 1 || tokens[0].type != JSMN_OBJECT) {
        log::log("Error: constants: expected json object as first json token from server.");
        exit(1);
    }

    if ((result % 2) != 1) {
        log::log("Error: constants: expected odd number of json tokens from server (root object + key/value pairs).");
        exit(1);
    }

    std::unordered_map<std::string, std::string> constants_map;

    for (int i = 1; i < result; i+=2) {
        std::string key = std::string(input + tokens[i].start, (size_t)tokens[i].end - (size_t)tokens[i].start);
        std::string value = std::string(input + tokens[i+1].start, (size_t)tokens[i+1].end - (size_t)tokens[i+1].start);
        constants_map[key] = value;
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
    CAPTURE_ENABLED = get_bool(constants_map, "CAPTURE_ENABLED");
    CAPTURE_RADIUS = get_int(constants_map, "CAPTURE_RADIUS");
    CAPTURE_SHIP_ADVANTAGE = get_int(constants_map, "SHIPS_ABOVE_FOR_CAPTURE");
}
