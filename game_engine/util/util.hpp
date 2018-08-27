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

/**
 * Combine hash values for two hashes.
 * @param lhs The first hash value.
 * @param rhs The second hash value.
 * @return The combined hash value.
 */
inline size_t hash_combine(size_t lhs, size_t rhs) {
    lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2); // NOLINT
    return lhs;
}

#endif // UTIL_HPP
