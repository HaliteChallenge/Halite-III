#include "NetworkingError.hpp"
#include "TimeoutError.hpp"
#include "WinConnection.hpp"

/** The number of slots in a pipe array. */
constexpr auto PIPE_PAIR = 2;
/** The index of the head of the pipe. */
constexpr auto PIPE_HEAD = 0;
/** The index of the tail of the pipe. */
constexpr auto PIPE_TAIL = 1;
/** The size of the pipe buffer. */
constexpr auto PIPE_BUFFER_SIZE = 64 * 1024;

namespace net {

/**
 * Initialize a WinConnection to a new process using a command.
 * Spawns the process, and binds two pipes to it for I/O.
 * @throws NetworkingError if the connection could not be established.
 * @param command The command to launch.
 * @param config The network configuration.
 */
WinConnection::WinConnection(const std::string &command, NetworkingConfig config) {
    this->config = config;
    HANDLE write[PIPE_PAIR];
    HANDLE read[PIPE_PAIR];

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = true;
    saAttr.lpSecurityDescriptor = nullptr;

    // Child stdout pipe
    if (!CreatePipe(&read[PIPE_HEAD], &read[PIPE_TAIL], &saAttr, PIPE_BUFFER_SIZE)) {
        throw NetworkingError("Could not create pipe");
    }
    if (!SetHandleInformation(read[PIPE_HEAD], HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(read[PIPE_HEAD]);
        CloseHandle(read[PIPE_TAIL]);
        throw NetworkingError("Could not set handle information");
    }

    // Child stdin pipe
    if (!CreatePipe(&write[PIPE_HEAD], &write[PIPE_TAIL], &saAttr, PIPE_BUFFER_SIZE)) {
        CloseHandle(read[PIPE_HEAD]);
        CloseHandle(read[PIPE_TAIL]);
        throw NetworkingError("Could not create pipe");
    }
    if (!SetHandleInformation(write[PIPE_TAIL], HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(read[PIPE_HEAD]);
        CloseHandle(read[PIPE_TAIL]);
        CloseHandle(write[PIPE_HEAD]);
        CloseHandle(write[PIPE_TAIL]);
        throw NetworkingError("Could not set handle information");
    }

    // MAKE SURE THIS MEMORY IS ERASED
    PROCESS_INFORMATION piProcInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    STARTUPINFO siStartInfo;
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = read[PIPE_TAIL];
    siStartInfo.hStdOutput = read[PIPE_TAIL];
    siStartInfo.hStdInput = write[PIPE_HEAD];
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    bool success = CreateProcess(
            "C:\\windows\\system32\\cmd.exe",
            LPSTR(("/C " + command).c_str()),     // command line
            nullptr,          // process security attributes
            nullptr,          // primary thread security attributes
            true,             // handles are inherited
            0,                // creation flags
            nullptr,          // use parent's environment
            nullptr,          // use parent's current directory
            &siStartInfo,     // STARTUPINFO pointer
            &piProcInfo       // receives PROCESS_INFORMATION
    );
    if (!success) {
        CloseHandle(read[PIPE_HEAD]);
        CloseHandle(read[PIPE_TAIL]);
        CloseHandle(write[PIPE_HEAD]);
        CloseHandle(write[PIPE_TAIL]);
        throw NetworkingError("Could not start process");
    }
    CloseHandle(piProcInfo.hThread);
    read_pipe = read[PIPE_HEAD];
    write_pipe = write[PIPE_TAIL];
    CloseHandle(read[PIPE_TAIL]);
    CloseHandle(write[PIPE_HEAD]);
    process = piProcInfo.hProcess;
}

/**
 * Destroy the WinConnection, terminating the subprocess if there is one.
 */
WinConnection::~WinConnection() noexcept {
    if (read_pipe != INVALID_HANDLE_VALUE) {
        CloseHandle(read_pipe);
    }
    if (write_pipe != INVALID_HANDLE_VALUE) {
        CloseHandle(write_pipe);
    }
    if (process != INVALID_HANDLE_VALUE) {
        TerminateProcess(process, 0);
        CloseHandle(process);
    }
}

/**
 * Send a string along this connection.
 * @param message The string to send.
 */
void WinConnection::send_string(const std::string &message) {
    DWORD chars_written;
    bool success = WriteFile(write_pipe, message.c_str(), message.length(), &chars_written, nullptr);
    if (!success || chars_written == 0) {
        throw NetworkingError("Problem writing to pipe");
    }
}

/**
 * Get a string from this connection.
 * @param timeout The timeout to use.
 * @return The string read.
 */
std::string WinConnection::get_string(std::chrono::milliseconds timeout) {
    char buffer;
    std::string result;

    using namespace std::chrono;
    auto initial_time = high_resolution_clock::now();
    while (true) {
        DWORD bytes_available = 0;
        while (true) {
            if (!config.ignore_timeout) {
                auto current_time = high_resolution_clock::now();
                auto remaining = timeout - duration_cast<milliseconds>(current_time - initial_time);
                if (remaining < milliseconds::zero()) {
                    throw TimeoutError("when reading string", timeout, result);
                }
            }

            PeekNamedPipe(read_pipe, nullptr, 0, nullptr, &bytes_available, nullptr);

            if (bytes_available > 0) {
                break;
            }

            Sleep(1);
        }

        DWORD chars_read;
        auto success = ReadFile(read_pipe, &buffer, 1, &chars_read, nullptr);
        if (!success || chars_read < 1) {
            throw NetworkingError("Could not read from pipe", result);
        }
        if (buffer == '\n') {
            break;
        } else {
            result += buffer;
        }
    }
    return result;
}

/**
 * Get the error output from this connection.
 * @return The error output.
 */
std::string WinConnection::get_errors() {
    return std::string();
}

}
