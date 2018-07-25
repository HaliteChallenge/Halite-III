#ifndef ENUMERATED_HPP
#define ENUMERATED_HPP

#include <iostream>
#include <unordered_map>
#include <utility>

#include "nlohmann/json.hpp"

/** The internal ID type. */
using id_value_type = long;

/** Sentinel value. */
static constexpr id_value_type NONE = -1;

/**
 * Type for IDs, ensuring IDs of different objects are not interchangeable.
 * No accidentally using Entity IDs for Players, or vice versa!
 * Should take no more memory than id_value_type to store.
 *
 * @tparam T The class to which these IDs belong.
 */
template<class T>
struct class_id final {
    /** Internal value. */
    id_value_type value;

    /** Equality test. */
    friend constexpr bool operator==(const class_id &first, const class_id &second) {
        return first.value == second.value;
    }

    /** Inequality test. */
    friend constexpr bool operator!=(const class_id &first, const class_id &second) {
        return !(first == second);
    }

    /** Comparison function. */
    friend constexpr bool operator<(const class_id &first, const class_id &second) {
        return first.value < second.value;
    }

    /** Stream input. */
    friend std::istream &operator>>(std::istream &istream, class_id &id) {
        return istream >> id.value;
    }

    /** Stream output. */
    friend std::ostream &operator<<(std::ostream &ostream, const class_id &id) {
        return ostream << id.value;
    }

    /** JSON serialization. */
    friend void to_json(nlohmann::json &json, const class_id &id) {
        nlohmann::to_json(json, id.value);
    }

    /** JSON deserialization. */
    friend void from_json(const nlohmann::json &json, class_id &id) {
        nlohmann::from_json(json, id.value);
    }

    /** String conversion. */
    friend std::string to_string(const class_id &id) {
        return std::to_string(id.value);
    }

    /** Default constructor. */
    constexpr class_id() = default;

    /** Explicit constructor. */
    constexpr explicit class_id(id_value_type value) : value(value) {}
};

/**
 * Mixin for classes exporting a numeric ID type.
 * @tparam T The class to which to add the ID.
 */
template<class T>
class Enumerated {
public:
    /** Alias for ID type. */
    using id_type = class_id<T>;

    /** Sentinel value for ID type. */
    static constexpr id_type None{NONE};

    /** Instance ID. */
    id_type id;

    /** Test two instances for equality. */
    bool operator==(const Enumerated<T> &other) const { return id == other.id; }

    /** Order two instances by ID. */
    bool operator<(const Enumerated<T> &other) const { return id < other.id; }

    /**
     * Construct with ID.
     * @param id The ID.
     */
    explicit Enumerated(id_type id) : id(id) {}

    /** Virtual destructor. */
    virtual ~Enumerated() = default;
};

namespace std {
template<class T>
struct hash<class_id<T>> {
    size_t operator()(const class_id<T> &object) const {
        return (size_t) object.value;
    }
};

template<class T>
struct hash<Enumerated<T>> {
    size_t operator()(const Enumerated<T> &object) const {
        return (size_t) object.id.value;
    }
};
}

/** Type of ordered maps from ID to arbitrary value. */
template<class K, class V>
using ordered_id_map = std::map<typename K::id_type, V>;

/** Type of maps from ID to arbitrary value. */
template<class K, class V>
using id_map = std::unordered_map<typename K::id_type, V>;

/**
 * Generic factory class that creates classes with an enumerated ID.
 * @tparam T The class to instantiate.
 */
template<class T>
class Factory final {
    /** The last ID allocated. */
    id_value_type last_id = T::None.value;
public:
    /**
     * Construct the object with a new ID.
     * @tparam Args The types of the constructor arguments.
     * @param args The constructor arguments.
     * @return The new object.
     */
    template<typename... Args>
    T make(Args &&...args) {
        return T(class_id<T>(++last_id), std::forward<Args>(args)...);
    }
};

#endif // ENUMERATED_HPP
