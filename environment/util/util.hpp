#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>

/**
 * Conversion in lieu of easy-to-access conversion from char to std::string.
 * @param character The character to convert to string.
 * @return The converted character.
 */
inline std::string to_string(char character) {
    return std::string() + character;
}

#endif // UTIL_HPP
