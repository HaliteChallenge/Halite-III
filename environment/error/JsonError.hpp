#ifndef JSONERROR_HPP
#define JSONERROR_HPP

#include <exception>
#include <string>
#include <utility>

#include "nlohmann/json.hpp"

/** Thrown when JSON cannot be decoded. */
class JsonError : public std::exception {
    /** The JSON. */
    nlohmann::json json;
public:
    /**
     * Construct JsonError from JSON input.
     * @param json The JSON input.
     */
    explicit JsonError(nlohmann::json json) : json(std::move(json)) {}

    /**
     * Get the exception description.
     * @return The exception description.
     */
    const char *what() const noexcept override {
        return ("Failed to decode JSON: " + json.dump()).c_str();
    }

};

#endif // JSONERROR_HPP
