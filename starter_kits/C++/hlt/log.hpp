#pragma once

#include <string>

namespace hlt {
    namespace log {
        void open(int bot_id);
        void log(const std::string& message);
    }
}
