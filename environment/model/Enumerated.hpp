#ifndef ENUMERATED_HPP
#define ENUMERATED_HPP

#include <utility>

/**
 * Mixin for classes exporting a numeric ID type.
 * @tparam T The class to which to add the ID.
 */
template<class T>
class Enumerated {
public:
    /** ID type. */
    using id_type = long;

    /** Sentinel value for ID type. */
    static constexpr id_type None = 0;

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
};

namespace std {
template<class T>
struct hash<Enumerated<T>> {
    size_t operator()(const Enumerated<T> &object) const {
        return (size_t) object.id;
    }
};
}

/**
 * Generic factory class that creates classes with an enumerated ID.
 * @tparam T The class to instantiate.
 */
template<class T>
class Factory {
    /** The last ID allocated. */
    typename T::id_type last_id = T::None;
public:
    /**
     * Construct the object with a new ID.
     * @tparam Args The types of the constructor arguments.
     * @param args The constructor arguments.
     * @return The new object.
     */
    template<typename... Args>
    T make(Args &&...args) {
        return T(++last_id, std::forward<Args>(args)...);
    }
};

#endif // ENUMERATED_HPP
