#include <iostream>

#include "Logging.hpp"

std::mutex Logging::cerr_mutex;

/** We default to the Info level of logging so that the user may see feedback. */
Logging::Level Logging::level = Logging::Level::Info;

void Logging::set_level(Logging::Level level) {
    Logging::level = level;
}

void Logging::log(const std::string &msg, Logging::Level level) {
    std::lock_guard<std::mutex> guard(Logging::cerr_mutex);
    // Only log at sufficiently high levels, otherwise suppress.
    if (static_cast<int>(level) >= static_cast<int>(Logging::level)) {
        switch (Logging::level) {
        case Logging::Level::Debug:
            std::cerr << "[DEBUG] ";
            break;
        case Logging::Level::Info:
            std::cerr << "[INFO] ";
            break;
        case Logging::Level::Warning:
            std::cerr << "[WARNING] ";
            break;
        case Logging::Level::Error:
            std::cerr << "[ERROR] ";
            break;
        }
        std::cerr << msg << std::endl;
    }
}
