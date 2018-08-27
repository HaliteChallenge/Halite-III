#ifndef BOTERROR_HPP
#define BOTERROR_HPP

#include <chrono>
#include <exception>
#include <string>

/** Abstract base class for an error a bot can make. */
struct BotError : public std::exception {
};

#endif // BOTERROR_HPP
