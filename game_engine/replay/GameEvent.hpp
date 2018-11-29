#ifndef GAMEEVENT_HPP
#define GAMEEVENT_HPP

#include "Location.hpp"
#include "Entity.hpp"
#include "Player.hpp"
#include "Store.hpp"
#include "Map.hpp"
#include "Statistics.hpp"

#include "nlohmann/json_fwd.hpp"

namespace hlt {

class BaseEvent;

/** Container type for events, exposed to Replay */
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
    Location location;      /**< Location of event  */

    /**
     * JSON encoding function, dispatched on game event subtypes.
     * @param[out] json The JSON output.
     */
    virtual void to_json(nlohmann::json &json) const = 0;

    BaseEvent(Location location) : location(location) {};

    virtual ~BaseEvent() = default;
    virtual void update_stats(const Store &store, const Map &map, GameStatistics &stats) {
        (void) store;
        (void) map;
        (void) stats;
    }
};

/** An event for entity spawning */
class SpawnEvent : public BaseEvent {
    energy_type energy;                                     /**< Energy granted to entity or of new entity in spawn */
    Player::id_type owner_id;                               /**< Id of player spawning entity */
    Entity::id_type id;                                     /**< ID of spawned entity */
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
    SpawnEvent(Location location, energy_type energy, Player::id_type owner_id, Entity::id_type id) :
            BaseEvent(location), energy(energy), owner_id(owner_id), id(id) {};
    ~SpawnEvent() override = default;

    virtual void update_stats(const Store &store, const Map &map, GameStatistics &stats) override;
};

/** An event for entity captures */
class CaptureEvent : public BaseEvent {
    Player::id_type old_owner;                              /**< ID of player giving entity */
    Entity::id_type old_id;                                 /**< ID of lost entity */
    Player::id_type new_owner;                              /**< ID of player receiving entity */
    Entity::id_type new_id;                                 /**< ID of gained entity */
    static constexpr auto GAME_EVENT_TYPE_NAME = "capture"; /**< Name of event */

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
    CaptureEvent(Location location,
                 Player::id_type old_owner, Entity::id_type old_id,
                 Player::id_type new_owner, Entity::id_type new_id) :
        BaseEvent(location),
        old_owner{old_owner}, old_id{old_id},
        new_owner{new_owner}, new_id{new_id} {};
    ~CaptureEvent() override = default;
};

/** An event for entity deaths */
class CollisionEvent : public BaseEvent {
    std::vector<Entity::id_type> ships;   /**< ids of entities involved in the collision */
    static constexpr auto GAME_EVENT_TYPE_NAME = "shipwreck"; /**< Name of event */

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
    CollisionEvent(Location location, std::vector<Entity::id_type> ships) : BaseEvent(location), ships(ships) {};
    ~CollisionEvent() override  = default;

    virtual void update_stats(const Store &store, const Map &map, GameStatistics &stats) override;
};

/** An event for Dropoff construction */
class ConstructionEvent : public BaseEvent {
    Player::id_type owner_id;   /**< ID of owner of dropoff point */
    Entity::id_type id;   /**< ID of ship being transformed into dropoff */
    static constexpr auto GAME_EVENT_TYPE_NAME = "construct"; /**< Name of event */

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
    ConstructionEvent(Location location, Player::id_type owner_id, Entity::id_type id) :
            BaseEvent(location), owner_id(owner_id), id(id) {};
    ~ConstructionEvent() override  = default;
};

}

#endif // GAMEEVENT_HPP
