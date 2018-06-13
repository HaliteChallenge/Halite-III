#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace hlt {

/** Global configuration. */
struct Config {
    long id;
    unsigned int seed;
    bool quiet_output;
    bool always_log;
    bool ignore_timeout;
    bool enable_replay;
    bool enable_compression;
};

}

#endif // CONFIG_HPP
