#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <utility>

/** Interface for classes exporting a numeric ID type. */
class Enumerated {
public:
    /** ID type. */
    using id_type = long;

    /** Sentinel value for ID type. */
    static constexpr id_type None = 0;

    /** Instance ID. */
    id_type id;

    /** Default constructor. */
    Enumerated() = default;

    /**
     * Construct with ID.
     * @param id The ID.
     */
    explicit Enumerated(id_type id) : id(id) {}
};

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

#endif // FACTORY_HPP
