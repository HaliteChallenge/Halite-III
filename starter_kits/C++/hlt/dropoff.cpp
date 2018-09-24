#include "dropoff.hpp"
#include "input.hpp"

std::shared_ptr<hlt::Dropoff> hlt::Dropoff::_generate(hlt::PlayerId player_id) {
    hlt::EntityId dropoff_id;
    int x;
    int y;
    hlt::get_sstream() >> dropoff_id >> x >> y;

    return std::make_shared<hlt::Dropoff>(player_id, dropoff_id, x, y);
}
