// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_LEXER_HPP
#define VUG_LEXER_HPP

#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

#include "Diagnostic/Logger.hpp"
#include "Misc/SourceManager.hpp"
#include "Token.hpp"

class Lexer {
public:
    explicit Lexer(const SourceFile& source) : _keywords({
                                                       {"mod", LexemType::Mod},
                                                       {"func", LexemType::Func},
                                                       {"var", LexemType::Var},
                                                       {"if", LexemType::If},
                                                       {"else", LexemType::Else},
                                                       {"while", LexemType::While},
                                                       {"break", LexemType::Break},
                                                       {"return", LexemType::Return},
                                               }),
                                               _source(source) {}

    bool match(const Token& token) {
        return getToken() == token;
    }

    Token getToken();
    void getTokens(std::vector<Token>& tokens);

    void revertTo(const Token& token);

protected:
    std::unordered_map<std::string, LexemType> _keywords;
    int64_t _pos{0};
    int64_t _line{1};
    int64_t _column{0};
    const SourceFile& _source;

    char peek() {
        ++_pos;
        ++_column;
        if (_pos <= static_cast<int64_t>(_source.getText().size())) {
            return _source.getText()[_pos - 1];
        }
        return '\0';
    }
    [[nodiscard]] char peekCurrent() const {
        if (_pos <= static_cast<int64_t>(_source.getText().size())) {
            return _source.getText()[_pos - 1];
        }
        return '\0';
    }

    Token getString();

    Token getIdentifier(char firstChar);

    Token getNumber(char firstChar);

    inline int64_t getPrevPos() const {
        return _pos - 1;
    }
};


#endif//VUG_LEXER_HPP
