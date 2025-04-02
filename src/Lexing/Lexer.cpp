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
                return {LexemType::LeftRoundBracket,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case ')':
                return {LexemType::RightRoundBracket,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case '{':
                return {LexemType::LeftCurlyBracket,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case '}':
                return {LexemType::RightCurlyBracket,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case '+':
                return {LexemType::Plus,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case '-':
                if (peek() == '>') {
                    return {LexemType::Arrow,
                            SourceLocation(getPrevPos() - 1, getPrevPos(), _line, _line, _column - 1, _column)};
                } else {
                    --_pos;
                    return {LexemType::Minus,
                            SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
                }
            case '*':
                return {LexemType::Multiply,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case '/':
                return {LexemType::Divide,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case '%':
                return {LexemType::Remainder,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case '!':
                if (peek() == '=') {
                    return {LexemType::Unequal,
                            SourceLocation(getPrevPos() - 1, getPrevPos(), _line, _line, _column - 1, _column)};
                } else {
                    --_pos;
                    return {LexemType::Not,
                            SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
                }
            case '|':
                if (peek() == '|') {
                    return {LexemType::LogicOr,
                            SourceLocation(getPrevPos() - 1, getPrevPos(), _line, _line, _column - 1, _column)};
                } else {
                    --_pos;
                    return {LexemType::BitOr,
                            SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
                }
            case '&':
                if (peek() == '&') {
                    return {LexemType::LogicAnd,
                            SourceLocation(getPrevPos() - 1, getPrevPos(), _line, _line, _column - 1, _column)};
                } else {
                    --_pos;
                    return {LexemType::BitAnd,
                            SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
                }
            case '^':
                return {LexemType::BitXor,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case '<':
                if (peek() == '=') {
                    return {LexemType::LessEqual,
                            SourceLocation(getPrevPos() - 1, getPrevPos(), _line, _line, _column - 1, _column)};
                } else {
                    --_pos;
                    return {LexemType::Less,
                            SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
                }
            case '>':
                if (peek() == '=') {
                    return {LexemType::GreaterEqual,
                            SourceLocation(getPrevPos() - 1, getPrevPos(), _line, _line, _column - 1, _column)};
                } else {
                    --_pos;
                    return {LexemType::Greater,
                            SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
                }
            case '=':
                if (peek() == '=') {
                    return {LexemType::Equal,
                            SourceLocation(getPrevPos() - 1, getPrevPos(), _line, _line, _column - 1, _column)};
                } else {
                    --_pos;
                    return {LexemType::Assign,
                            SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
                }
            case ',':
                return {LexemType::Comma,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case ';':
                return {LexemType::Semicolon,
                        SourceLocation(getPrevPos(), getPrevPos(), _line, _line, _column, _column)};
            case '"':
                return getString();

            case ' ':
            case '\r':
                break;
            case '\t':
                _column += 4;
                break;
            case '\n':
                _column = 0;
                ++_line;
                break;

            case '\0':
                return {LexemType::EndOfFile,
                        SourceLocation()};

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

    auto startPos = getPrevPos();
    auto startLine = _line;
    auto startColumn = _column;

    while (true) {
        char c = peek();

        if (c == '\0') {
            throw std::logic_error("Bad lexing");
        }
        if (c == '\r') {
            continue;
        }
        if (c == '\n') {
            _column = 0;
            ++_line;
        }

        if (c == '"') {
            break;
        }

        string.push_back(c);
    }
    return {LexemType::String,
            SourceLocation(startPos, getPrevPos(),
                           startLine, getPrevPos(),
                           startColumn, getPrevPos()),
            std::move(string)};
}
Token Lexer::getIdentifier(char firstChar) {
    auto startPos = getPrevPos();
    auto startLine = _line;
    auto startColumn = _column;

    std::string string;
    string.push_back(firstChar);

    while (true) {
        char c = peek();

        if (!isalpha(c) && !isdigit(c)) {
            --_pos;
            --_column;
            break;
        }

        string.push_back(c);
    }

    auto keyword = _keywords.find(string);

    if (keyword != _keywords.end()) {
        return {
                keyword->second,
                SourceLocation(startPos, getPrevPos(),
                               startLine, _line,
                               startColumn, _column),
        };
    } else {
        return {LexemType::Identifier,
                SourceLocation(startPos, _pos,
                               startLine, _line,
                               startColumn, _column),
                std::move(string)};
    }
}
Token Lexer::getNumber(char firstChar) {
    std::string string;
    string.push_back(firstChar);

    auto startPos = getPrevPos();
    auto startLine = _line;
    auto startColumn = _column;

    while (true) {
        char c = peek();

        if (!isdigit(c)) {
            --_pos;
            --_column;
            break;
        }

        string.push_back(c);
    }

    return {LexemType::Number,
            SourceLocation(startPos, getPrevPos(),
                           startLine, _line,
                           startColumn, _column),
            std::move(string)};
}
