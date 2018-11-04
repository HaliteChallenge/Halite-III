#include "Halite.hpp"
#include "HaliteImpl.hpp"

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
        .constructor<GameStatistics&, size_t, unsigned int, const Map&>();

    enum_<mapgen::MapType>("MapgenMapType")
        .value("Fractal", mapgen::MapType::Fractal);

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
        .function("end_turn", &HaliteImpl::end_turn)
        .function("end_game", &HaliteImpl::end_game)
        .function("process_turn_from_input", &HaliteImpl::process_turn_from_input)
        ;

    class_<Halite>("Halite")
        .constructor<Map&, const net::NetworkingConfig&, GameStatistics&, Replay&>()
        .function("get_impl", &Halite::get_impl)
        .function("get_networking", &Halite::get_networking, allow_raw_pointers());
}
