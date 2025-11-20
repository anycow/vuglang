// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_LOGGER_HPP
#define VUG_LOGGER_HPP

#include <cstdint>
#include <iostream>
#include <string>

enum class LogLevel : uint32_t {
    Fatal,
    Error,
    Warning,
    Note,
    Verbose,
};

template<LogLevel RequiredLevel>
class Logger {
public:
    Logger() = default;

    template<LogLevel Level>
    inline void log(const std::string& message) const {
        if constexpr (Level <= RequiredLevel) {
            std::cout << levelToString(Level) << ": "
                      << message
                      << std::endl;

            if constexpr (Level == LogLevel::Fatal) {
                std::exit(EXIT_FAILURE);
            }
        }
    }

protected:
    [[nodiscard]] inline constexpr const char* levelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Fatal:
                return "Fatal";
            case LogLevel::Error:
                return "Error";
            case LogLevel::Warning:
                return "Warning";
            case LogLevel::Note:
                return "Note";
            case LogLevel::Verbose:
                return "Verbose";
            default:
                return "Unknown";
        }
    }
};


#endif//VUG_LOGGER_HPP
