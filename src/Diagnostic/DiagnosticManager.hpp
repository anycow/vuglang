// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_DIAGNOSTICMANAGER_HPP
#define VUG_DIAGNOSTICMANAGER_HPP

#include "Diagnostic/Diagnostic.hpp"

class DiagnosticManager {
   public:
    explicit DiagnosticManager(DiagnosticMessage::Severity requiredSeverity)
        : _requiredSeverity(requiredSeverity) {
    }

    void report(const Diagnostic& diagnostic);

    [[nodiscard]] DiagnosticMessage::Severity required_severity() const {
        return _requiredSeverity;
    }
    [[nodiscard]] uint32_t fatal_count() const {
        return _fatalCount;
    }
    [[nodiscard]] uint32_t error_count() const {
        return _errorCount;
    }
    [[nodiscard]] uint32_t warning_count() const {
        return _warningCount;
    }
    [[nodiscard]] uint32_t hint_count() const {
        return _hintCount;
    }
    [[nodiscard]] uint32_t info_count() const {
        return _infoCount;
    }

   private:
    DiagnosticMessage::Severity _requiredSeverity;

    uint32_t _fatalCount{};
    uint32_t _errorCount{};
    uint32_t _warningCount{};
    uint32_t _hintCount{};
    uint32_t _infoCount{};
};


#endif  // VUG_DIAGNOSTICMANAGER_HPP
