#ifndef GAMEEVENT_HPP
#define GAMEEVENT_HPP

#include "nlohmann/json_fwd.hpp"
#include "Location.hpp"
#include "Entity.hpp"

namespace hlt {

class BaseEvent;

/** Container type for cells, exposed to Replay */
using GameEvent = std::unique_ptr<BaseEvent>;

/**
 * Convert a GameEvent to JSON format.
 * @param[out] json The output JSON.
 * @param game_event The GameEvent to convert.
 */
void to_json(nlohmann::json &json, const GameEvent &gameEvent);

/**
* An event that happens during game simulation. Recorded for replay file to be used by visualizer.
*/
class BaseEvent {

public:
    /**
     * JSON encoding function, dispatched on game event subtypes.
     * @param[out] json The JSON output.
     */
    virtual void to_json(nlohmann::json &json) const = 0;

    virtual ~BaseEvent() = default;
};

/** An event for entity spawning */
class SpawnEvent : public BaseEvent {
    Location location;      /**< Location of spawn */
    energy_type energy;     /**< Energy granted to entity or of new entity in spawn */
    Player::id_type owner_id;       /**< Id of player spawning entity */
    static constexpr auto GAME_EVENT_TYPE_NAME = "spawn";   /**< Name of event */

public:
    /**
     * Convert spawn event to json format
     *
     * @param[out] json JSON to be filled by spawn event
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Create spawn event from location, energy, and owner id
     *
     * @param location Location of spawn
     * @param energy Energy associated with spawn
     * @param owner_id Id of owner who spawned this entity
     */
    SpawnEvent(Location location, energy_type energy, Player::id_type owner_id) :
            location(std::move(location)), energy(energy), owner_id(owner_id) {};
    ~SpawnEvent() override = default;
};

/** An event for entity deaths */
class DeathEvent : public BaseEvent {
    Location location;  /**< Location of entity death */
    Player::id_type owner_id;   /**< Owner of dying entity */
    static constexpr auto GAME_EVENT_TYPE_NAME = "death"; /**< Name of event */

public:
    /**
     * Convert death event to json format
     * @param[out] json JSON to be filled with death event
     */
    void to_json(nlohmann::json &json) const override;

    /**
     * Create death event from location and owner id
     * @param location Location of entity death
     * @param owner_id Owner of dying entity
     */
    DeathEvent(Location location, Player::id_type owner_id) : location(std::move(location)), owner_id(owner_id) {};
    ~DeathEvent() override  = default;
};

}

#endif // GAMEEVENT_HPP