#include "Halite.hpp"
#include "HaliteImpl.hpp"
#include "Logging.hpp"

#include <emscripten/bind.h>

using namespace hlt;
using namespace emscripten;

EMSCRIPTEN_BINDINGS(libhaliteweb) {
    class_<Map>("Map")
        .constructor<dimension_type, dimension_type>();
    class_<net::NetworkingConfig>("NetworkingConfig")
        .constructor<>();
    class_<GameStatistics>("GameStatistics")
        .constructor<>();
    class_<Replay>("Replay")
        .constructor<GameStatistics&, size_t, unsigned int, const Map&>()
        .function("as_json_string", &Replay::as_json_string)
        .function("turn_as_json_string", &Replay::turn_as_json_string)
        ;

    enum_<mapgen::MapType>("MapgenMapType")
        .value("Fractal", mapgen::MapType::Fractal);
    enum_<Logging::Level>("LoggingLevel")
        .value("Debug", Logging::Level::Debug)
        .value("Info", Logging::Level::Info)
        .value("Warning", Logging::Level::Warning)
        .value("Error", Logging::Level::Error)
        ;

    class_<Logging>("Logging")
        .class_function("set_level", &Logging::set_level)
        ;

    value_object<mapgen::MapParameters>("MapgenParameters")
        .field("type", &mapgen::MapParameters::type)
        .field("seed", &mapgen::MapParameters::seed)
        .field("width", &mapgen::MapParameters::width)
        .field("height", &mapgen::MapParameters::height)
        .field("num_players", &mapgen::MapParameters::num_players)
        ;
    class_<mapgen::Generator>("Mapgen")
        .class_function("generate", &mapgen::Generator::generate)
        ;

    class_<PlayerLog>("PlayerLog");
    class_<Player>("Player");
    class_<Snapshot>("Snapshot")
        .constructor<>();

    class_<Store>("Store")
        .function("get_player", &Store::get_player);

    class_<Player::id_type>("PlayerId")
        .constructor<long>();
    register_map<Player::id_type, std::string>("MapPlayerIdString");
    register_vector<std::string>("VectorString");

    class_<net::Networking>("Networking")
        .function("generate_player_init_input", &net::Networking::generate_player_init_input)
        .function("generate_player_input", &net::Networking::generate_player_input);

    class_<HaliteImpl>("HaliteImpl")
        .function("start_turn", &HaliteImpl::start_turn)
        .function("initialize_game", &HaliteImpl::initialize_game)
        .function("initialize_players", &HaliteImpl::initialize_players)
        .function("end_turn", &HaliteImpl::end_turn)
        .function("end_game", &HaliteImpl::end_game)
        .function("game_ended", &HaliteImpl::game_ended)
        .function("process_turn_from_input", &HaliteImpl::process_turn_from_input)
        .function("set_turn_number", &HaliteImpl::set_turn_number)
        .function("max_turns", &HaliteImpl::max_turns)
        ;

    class_<Halite>("Halite")
        .constructor<Map&, const net::NetworkingConfig&, GameStatistics&, Replay&>()
        .function("get_impl", &Halite::get_impl)
        .function("get_store", &Halite::get_store)
        .function("get_networking", &Halite::get_networking, allow_raw_pointers())
        ;
}
