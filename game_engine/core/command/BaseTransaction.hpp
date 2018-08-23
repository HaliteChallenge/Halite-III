#ifndef BASETRANSACTION_HPP
#define BASETRANSACTION_HPP

#include "CommandError.hpp"
#include "GameEvent.hpp"
#include "Location.hpp"

namespace hlt {

class Map;

class Store;

/** Base transaction class independent of commands. */
class BaseTransaction {
public:
    /** Callback type. */
    template<class... Ts>
    using callback = std::function<void(Ts...)>;

private:
    callback<GameEvent> event_callback;               /**< Game event callback. */
    callback<CommandError> error_callback;            /**< Error callback. */
    callback<Entity::id_type> entity_update_callback; /**< Entity update callback. */
    callback<Location> cell_update_callback;          /**< Cell update callback. */

protected:
    Store &store; /**< The game store. */
    Map &map;     /**< The game map. */

    /**
     * Process a generated event.
     * @param event The event.
     */
    void event_generated(GameEvent event) {
        if (event_callback) {
            event_callback(std::move(event));
        }
    }

    /**
     * Generate and process an event.
     * @tparam EventType The type of the event.
     * @tparam Args The type of the event constructor arguments.
     * @param args The event constructor arguments.
     */
    template<class EventType, class... Args>
    void event_generated(Args &&...args) {
        event_generated(std::make_unique<EventType>(std::forward<Args>(args)...));
    }

    /**
     * Process a generated error.
     * @param error The error.
     */
    void error_generated(CommandError error) {
        if (error_callback) {
            error_callback(std::move(error));
        }
    }

    /**
     * Generate and process an error.
     * @tparam ErrorType The type of the error.
     * @tparam Args The type of the error constructor arguments.
     * @param args The error constructor arguments.
     */
    template<class ErrorType, class... Args>
    void error_generated(Args &&...args) {
        error_generated(std::make_unique<ErrorType>(std::forward<Args>(args)...));
    }

    /**
     * Process an updated entity.
     * @param id The entity ID.
     */
    void entity_updated(Entity::id_type id) {
        if (entity_update_callback) {
            entity_update_callback(id);
        }
    }

    /**
     * Process an updated cell.
     * @param location The cell location.
     */
    void cell_updated(Location location) {
        if (cell_update_callback) {
            cell_update_callback(location);
        }
    }

public:
    /**
     * Construct BaseTransaction from Store and Map.
     * @param store The Store.
     * @param map The Map.
     */
    explicit BaseTransaction(Store &store, Map &map) : store(store), map(map) {}

    /**
     * Set a callback for events generated during the transaction commit.
     * @param callback The callback to set.
     */
    virtual void on_event(callback<GameEvent> callback) {
        event_callback = std::move(callback);
    }

    /**
     * Set a callback for errors generated in the transaction.
     * @param callback The callback to set.
     */
    virtual void on_error(callback<CommandError> callback) {
        error_callback = std::move(callback);
    }

    /**
     * Set a callback for entity updates in the transaction.
     * @param callback The callback to set.
     */
    virtual void on_entity_update(callback<Entity::id_type> callback) {
        entity_update_callback = std::move(callback);
    }

    /**
     * Set a callback for cell updates in the transaction.
     * @param callback The callback to set.
     */
    virtual void on_cell_update(callback<Location> callback) {
        cell_update_callback = std::move(callback);
    }

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    virtual bool check() = 0;

    /** If the transaction may be committed, commit the transaction. */
    virtual void commit() = 0;

    /** Virtual destructor. */
    virtual ~BaseTransaction() = default;
};

}

#endif // BASETRANSACTION_HPP
