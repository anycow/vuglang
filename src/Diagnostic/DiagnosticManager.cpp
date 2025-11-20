// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "DiagnosticManager.hpp"

#include "Misc/SourceManager.hpp"

void DiagnosticManager::report(const Diagnostic& diagnostic) {
    for (const auto& message : diagnostic.getMessages()) {
        switch (message.getSeverity()) {
            case DiagnosticMessage::Severity::Fatal:
                _fatalCount++;
                break;
            case DiagnosticMessage::Severity::Error:
                _errorCount++;
                break;
            case DiagnosticMessage::Severity::Warning:
                _warningCount++;
                break;
            case DiagnosticMessage::Severity::Hint:
                _hintCount++;
                break;
            case DiagnosticMessage::Severity::Info:
                _infoCount++;
                break;
        }

        if (message.getSeverity() <= _requiredSeverity) {
            std::cout << severityToString(message.getSeverity()) << ": " << message.getMessage()
                      << std::endl;

            for (const auto& code : message.getRelatedCode()) {
                if (code.isValid()) {
                    for (int64_t i = code.getStartLine(); i <= code.getEndLine(); ++i) {
                        auto line = code.getSourceFile()->getLine(i);

                        size_t start = 0;
                        for (; start < line.size()
                               && std::isspace(static_cast<unsigned char>(line[start]));
                             ++start) {
                        }

                        std::cout << std::setw(4) << i << "| " << line.substr(start) << std::endl;
                    }
                }
            }

            if (!message.getFixes().empty()) {
                std::cout << "probably fix:" << std::endl;
                for (const auto& fix : message.getFixes()) {
                    for (const auto& diff : fix.getDiffs()) {
                        auto line = diff.sourceFile.getLine(diff.sourceLine);
                        size_t start = 0;
                        for (; start < line.size()
                               && std::isspace(static_cast<unsigned char>(line[start]));
                             ++start) {
                        }
                        std::cout << "-" << std::setw(3) << diff.sourceLine << "| "
                                  << line.substr(start) << std::endl;
                    }

                    for (const auto& diff : fix.getDiffs()) {
                        auto line = diff.newString;
                        size_t start = 0;
                        for (; start < line.size()
                               && std::isspace(static_cast<unsigned char>(line[start]));
                             ++start) {
                        }
                        std::cout << "+" << std::setw(3) << diff.sourceLine << "| "
                                  << line.substr(start) << std::endl;
                    }
                }
            }

            std::cout << std::endl;
        }
    }
}
