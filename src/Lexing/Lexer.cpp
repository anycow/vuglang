
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Lexer.hpp"
#include "Token.hpp"
#include <stdexcept>

Token Lexer::getToken() {
    while (true) {
        auto c = peek();

        switch (c) {
            case '(':
                return {LexemType::LeftRoundBracket};
            case ')':
                return {LexemType::RightRoundBracket};
            case '{':
                return {LexemType::LeftCurlyBracket};
            case '}':
                return {LexemType::RightCurlyBracket};
            case '+':
                return {LexemType::Plus};
            case '-':
                if (peek() == '>') {
                    return {LexemType::Arrow};
                } else {
                    --_pos;
                    return {LexemType::Minus};
                }
            case '*':
                return {LexemType::Multiply};
            case '/':
                return {LexemType::Divide};
            case '%':
                return {LexemType::Remainder};
            case '!':
                if (peek() == '=') {
                    return {LexemType::Unequal};
                } else {
                    --_pos;
                    return {LexemType::Not};
                }
            case '|':
                if (peek() == '|') {
                    return {LexemType::LogicOr};
                } else {
                    --_pos;
                    return {LexemType::BitOr};
                }
            case '&':
                if (peek() == '&') {
                    return {LexemType::LogicAnd};
                } else {
                    --_pos;
                    return {LexemType::BitAnd};
                }
            case '^':
                return {LexemType::BitXor};
            case '<':
                if (peek() == '=') {
                    return {LexemType::LessEqual};
                } else {
                    --_pos;
                    return {LexemType::Less};
                }
            case '>':
                if (peek() == '=') {
                    return {LexemType::GreaterEqual};
                } else {
                    --_pos;
                    return {LexemType::Greater};
                }
            case '=':
                if (peek() == '=') {
                    return {LexemType::Equal};
                } else {
                    --_pos;
                    return {LexemType::Assign};
                }
            case ',':
                return {LexemType::Comma};
            case ';':
                return {LexemType::Semicolon};
            case '"':
                return getString();

            case ' ':
            case '\t':
            case '\n':
            case '\r':
                break;

            case '\0':
                return {LexemType::EndOfFile};

            default:
                if (c >= '0' && c <= '9') {
                    return getNumber(c);
                } else {
                    return getIdentifier(c);
                }
        }
    }
}
void Lexer::getTokens(std::vector<Token>& tokens) {
    while (true) {
        auto token = getToken();

        tokens.emplace_back(token);

        if (token.getType() == LexemType::EndOfFile) {
            _pos = 0;
            return;
        }
    }
}
Token Lexer::getString() {
    std::string string;

    while (true) {
        char c = peek();

        if (c == '\0') {
            throw std::logic_error("Bad lexing");
        }
        if (c == '\r') {
            continue;
        }

        if (c == '"') {
            break;
        }

        string.push_back(c);
    }
    return {LexemType::String, std::move(string)};
}
Token Lexer::getIdentifier(char firstChar) {
    std::string string;
    string.push_back(firstChar);

    while (true) {
        char c = peek();

        if (!isalpha(c) && !isdigit(c)) {
            _pos--;
            break;
        }

        string.push_back(c);
    }

    auto keyword = _keywords.find(string);

    if (keyword != _keywords.end()) {
        return {keyword->second};
    } else {
        return {LexemType::Identifier, std::move(string)};
    }
}
Token Lexer::getNumber(char firstChar) {
    std::string string;
    string.push_back(firstChar);

    while (true) {
        char c = peek();

        if (!isdigit(c)) {
            _pos--;
            break;
        }

        string.push_back(c);
    }

    return {LexemType::Number, std::move(string)};
}
