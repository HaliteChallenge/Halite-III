#pragma once

#include <string>
#include <iostream>
#include <sstream>

namespace hlt {
    static std::string get_string() {
        std::string result;
        std::getline(std::cin, result);
        return result;
    }

    static std::stringstream get_sstream() {
        return std::stringstream(get_string());
    }
}
