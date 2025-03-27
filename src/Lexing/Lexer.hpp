// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_LEXER_HPP
#define VUG_LEXER_HPP

#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

#include "Token.hpp"

class Lexer {
public:
    explicit Lexer(const std::string& source) : _keywords({
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

    inline char peek() {
        pos++;
        if (pos <= _source.size()) {
            return _source[pos - 1];
        }
        return '\0';
    }

    inline char peekCurrent() {
        if (pos <= _source.size()) {
            return _source[pos - 1];
        }
        return '\0';
    }

    inline bool match(const Token& token) {
        return getToken() == token;
    }

    Token getToken();

    void getTokens(std::vector<Token>& tokens);

protected:
    std::unordered_map<std::string, LexemType> _keywords;

    size_t pos = 0;
    const std::string& _source;

    Token getString();

    Token getIdentifier(char firstChar);

    Token getNumber(char firstChar);
};


#endif//VUG_LEXER_HPP
