#ifndef SNAPSHOTERROR_HPP
#define SNAPSHOTERROR_HPP

#include <exception>
#include <string>
#include <cstring>
#include <sstream>

/** Thrown when a snapshot cannot be decoded. */
class SnapshotError : public std::exception {
    std::string buffer;      /**< The message buffer. */

public:
    /**
     * Construct SnapshotError from description and position.
     * @param description The error description.
     * @param position The decoding position.
     */
    SnapshotError(const std::string &description, int position) {
        std::stringstream result;
        result << "could not decode snapshot: ";
        result << description;
        result << " (at position ";
        result << position;
        result << ")";
        buffer = result.str();
    }

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        return buffer.c_str();
    }

};

#endif // SNAPSHOTERROR_HPP
