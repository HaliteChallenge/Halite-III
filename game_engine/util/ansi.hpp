#ifndef ANSI_HPP
#define ANSI_HPP

/** ANSI terminal colors. */
namespace ansi {
static constexpr const char *red = "\033[0;31m";
static constexpr const char *green = "\033[0;32m";
static constexpr const char *yellow = "\033[0;33m";
static constexpr const char *cyan = "\033[0;36m";
static constexpr const char *magenta = "\033[0;35m";
static constexpr const char *reset = "\033[0m";
static constexpr const char *bold_red = "\033[1;31m";
static constexpr const char *bold_green = "\033[1;32m";
static constexpr const char *bold_yellow = "\033[1;33m";
static constexpr const char *bold_cyan = "\033[1;36m";
static constexpr const char *bold_magenta = "\033[1;35m";
}

#endif // ANSI_HPP
