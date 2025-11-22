// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_DIAGNOSTICMANAGER_HPP
#define VUG_DIAGNOSTICMANAGER_HPP

#include "Diagnostic/Diagnostic.hpp"

class DiagnosticManager {
   public:
    constexpr explicit DiagnosticManager(const DiagnosticMessage::Severity requiredSeverity)
        : mRequiredSeverity(requiredSeverity) {
    }

    void report(const Diagnostic& diagnostic);

    [[nodiscard]] constexpr DiagnosticMessage::Severity required_severity() const {
        return mRequiredSeverity;
    }
    [[nodiscard]] constexpr uint32_t fatal_count() const {
        return mFatalCount;
    }
    [[nodiscard]] constexpr uint32_t error_count() const {
        return mErrorCount;
    }
    [[nodiscard]] constexpr uint32_t warning_count() const {
        return mWarningCount;
    }
    [[nodiscard]] constexpr uint32_t hint_count() const {
        return mHintCount;
    }
    [[nodiscard]] constexpr uint32_t info_count() const {
        return mInfoCount;
    }

   private:
    const DiagnosticMessage::Severity mRequiredSeverity;

    uint32_t mFatalCount{};
    uint32_t mErrorCount{};
    uint32_t mWarningCount{};
    uint32_t mHintCount{};
    uint32_t mInfoCount{};
};


#endif  // VUG_DIAGNOSTICMANAGER_HPP
