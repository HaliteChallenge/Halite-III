#include "Connection.hpp"
#include "NetworkingError.hpp"
#include "TimeoutError.hpp"

namespace net {

/**
 * Read any remaining input from the pipe.
 * @return The remaining input.
 */
std::string BaseConnection::read_trailing_input() {
    std::string result;
    config.ignore_timeout = false;
    config.timeout = std::chrono::milliseconds(0);
    while (true) {
        try {
            result += get_string();
            result += "\n";
        }
        catch (const TimeoutError& err) {
            result += err.remaining_input;
            break;
        }
        catch (const NetworkingError& err) {
            result += err.remaining_input;
            break;
        }
        catch (...) {
            break;
        }
    }
    return result;
}

}
