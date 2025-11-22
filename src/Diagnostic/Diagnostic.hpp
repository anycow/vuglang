// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_DIAGNOSTIC_HPP
#define VUG_DIAGNOSTIC_HPP

#include <string>
#include <utility>

#include "Lexing/Token.hpp"

class DiagnosticFix {
   public:
    struct Diff {
        const SourceFile& sourceFile;
        const uint64_t sourceLine;
        const std::string newString;

        Diff(const SourceFile& sourceFile, const uint64_t sourceLine, std::string newString)
            : sourceFile(sourceFile),
              sourceLine(sourceLine),
              newString(std::move(newString)) {
        }
    };

    DiagnosticFix() = default;

    [[nodiscard]] const std::vector<Diff>& getDiffs() const {
        return mDiffs;
    }
    DiagnosticFix& addDiff(const SourceFile& sourceFile,
                           const uint64_t sourceLine,
                           std::string newString) {
        mDiffs.emplace_back(sourceFile, sourceLine, std::move(newString));
        return *this;
    }

   private:
    std::vector<Diff> mDiffs;
};

// TODO! use builder pattern
class DiagnosticMessage {
   public:
    enum class Severity { Fatal, Error, Warning, Hint, Info };

    constexpr explicit DiagnosticMessage(const Severity severity,
                                         std::string message,
                                         std::vector<SourceLocation> relatedCode)
        : mSeverity(severity),
          mMessage(std::move(message)),
          mRelatedCode(std::move(relatedCode)) {
    }

    [[nodiscard]] constexpr Severity getSeverity() const {
        return mSeverity;
    }

    [[nodiscard]] constexpr const std::string& getMessage() const {
        return mMessage;
    }

    [[nodiscard]] constexpr const std::vector<SourceLocation>& getRelatedCode() const {
        return mRelatedCode;
    }

    [[nodiscard]] constexpr const std::vector<DiagnosticFix>& getFixes() const {
        return mFixes;
    }
    constexpr DiagnosticMessage& addFix(const DiagnosticFix& fix) {
        mFixes.push_back(fix);
        return *this;
    }

   private:
    const Severity mSeverity;
    const std::string mMessage;
    std::vector<SourceLocation> mRelatedCode;
    std::vector<DiagnosticFix> mFixes;
};

class Diagnostic {
   public:
    constexpr Diagnostic() = default;

    void addMessage(const DiagnosticMessage& message) {
        mMessages.push_back(message);
    }
    [[nodiscard]] const std::vector<DiagnosticMessage>& getMessages() const {
        return mMessages;
    }

   private:
    std::vector<DiagnosticMessage> mMessages;
};

constexpr const char* severityToString(const DiagnosticMessage::Severity severity) {
    switch (severity) {
        case DiagnosticMessage::Severity::Fatal:
            return "fatal";
        case DiagnosticMessage::Severity::Error:
            return "error";
        case DiagnosticMessage::Severity::Warning:
            return "warning";
        case DiagnosticMessage::Severity::Hint:
            return "hint";
        case DiagnosticMessage::Severity::Info:
            return "info";
        default:
            return "unknown";
    }
}
#endif  // VUG_DIAGNOSTIC_HPP
