// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_DIAGNOSTICPRINTER_HPP
#define VUG_DIAGNOSTICPRINTER_HPP

#include <cstdint>
#include <iostream>
#include <string>

enum class DiagnosticLevel : uint32_t {
    Fatal,
    Error,
    Warning,
    Note,
    Verbose,
};

template<DiagnosticLevel MaxLevel>
class DiagnosticPrinter {
public:
    DiagnosticPrinter() = default;

    template<DiagnosticLevel Level>
    inline void log(const std::string& message) const {
        if constexpr (Level <= MaxLevel) {
            std::cout << levelToString(Level) << ": "
                      << message
                      << std::endl;

            if constexpr (Level == DiagnosticLevel::Fatal) {
                std::exit(EXIT_FAILURE);
            }
        }
    }

protected:
    [[nodiscard]] inline constexpr const char* levelToString(DiagnosticLevel level) const {
        switch (level) {
            case DiagnosticLevel::Fatal:
                return "Fatal";
            case DiagnosticLevel::Error:
                return "Error";
            case DiagnosticLevel::Warning:
                return "Warning";
            case DiagnosticLevel::Note:
                return "Note";
            case DiagnosticLevel::Verbose:
                return "Verbose";
            default:
                return "Unknown";
        }
    }
};


#endif//VUG_DIAGNOSTICPRINTER_HPP
