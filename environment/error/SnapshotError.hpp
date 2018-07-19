#ifndef SNAPSHOTERROR_HPP
#define SNAPSHOTERROR_HPP

#include <exception>
#include <string>
#include <cstring>
#include <sstream>

/** Thrown when a snapshot cannot be decoded. */
class SnapshotError : public std::exception {
    std::string description;
    int position;

public:
    explicit SnapshotError(const std::string description, int position) : description{description},
                                                                          position{position} {}

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        std::stringstream result;

        result << "Could not decode snapshot: ";
        result << description;
        result << " (at position ";
        result << position;
        result << ")";

        const std::string copy = result.str();
        return strdup(copy.c_str());
    }

};

#endif // SNAPSHOTERROR_HPP
