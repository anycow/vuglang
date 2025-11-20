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

        Diff(const SourceFile& sourceFile, uint64_t sourceLine, std::string newString)
            : sourceFile(sourceFile),
              sourceLine(sourceLine),
              newString(std::move(newString)) {
        }
    };

    DiagnosticFix() = default;

    [[nodiscard]] const std::vector<Diff>& getDiffs() const {
        return _diffs;
    }
    DiagnosticFix& addDiff(const SourceFile& sourceFile,
                           uint64_t sourceLine,
                           std::string newString) {
        _diffs.emplace_back(sourceFile, sourceLine, std::move(newString));
        return *this;
    }

   private:
    std::vector<Diff> _diffs;
};

// TODO! use builder pattern
class DiagnosticMessage {
   public:
    enum class Severity { Fatal, Error, Warning, Hint, Info };

    explicit DiagnosticMessage(const Severity severity,
                               std::string message,
                               std::vector<SourceLocation> relatedCode)
        : _severity(severity),
          _message(std::move(message)),
          _relatedCode(std::move(relatedCode)) {
    }

    [[nodiscard]] Severity getSeverity() const {
        return _severity;
    }

    [[nodiscard]] const std::string& getMessage() const {
        return _message;
    }

    [[nodiscard]] const std::vector<SourceLocation>& getRelatedCode() const {
        return _relatedCode;
    }

    [[nodiscard]] const std::vector<DiagnosticFix>& getFixes() const {
        return _fixes;
    }
    DiagnosticMessage& addFix(const DiagnosticFix& fix) {
        _fixes.push_back(fix);
        return *this;
    }

   private:
    const Severity _severity;
    const std::string _message;
    std::vector<SourceLocation> _relatedCode;
    std::vector<DiagnosticFix> _fixes;
};

class Diagnostic {
   public:
    Diagnostic() = default;

    void addMessage(const DiagnosticMessage& message) {
        _messages.push_back(message);
    }
    [[nodiscard]] const std::vector<DiagnosticMessage>& getMessages() const {
        return _messages;
    }

   private:
    std::vector<DiagnosticMessage> _messages;
};

constexpr const char* severityToString(DiagnosticMessage::Severity severity) {
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
