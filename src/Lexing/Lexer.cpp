// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "Lexer.hpp"
#include <stdexcept>
#include "Token.hpp"

Token Lexer::getToken() {
    while (true) {
        auto c = peek();

        switch (c) {
            case '(':
                return {LexemType::LeftRoundBracket,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case ')':
                return {LexemType::RightRoundBracket,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case '{':
                return {LexemType::LeftCurlyBracket,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case '}':
                return {LexemType::RightCurlyBracket,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case '+':
                return {LexemType::Plus,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case '-':
                if (peek() == '>') {
                    return {LexemType::Arrow,
                            SourceLocation(&_source,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column - 1,
                                           _column)};
                } else {
                    --_pos;
                    --_column;
                    return {LexemType::Minus,
                            SourceLocation(&_source,
                                           getPrevPos(),
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column,
                                           _column)};
                }
            case '*':
                return {LexemType::Multiply,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case '/':
                return {LexemType::Divide,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case '%':
                return {LexemType::Remainder,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case '!':
                if (peek() == '=') {
                    return {LexemType::Unequal,
                            SourceLocation(&_source,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column - 1,
                                           _column)};
                } else {
                    --_pos;
                    --_column;
                    return {LexemType::Not,
                            SourceLocation(&_source,
                                           getPrevPos(),
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column,
                                           _column)};
                }
            case '|':
                if (peek() == '|') {
                    return {LexemType::LogicOr,
                            SourceLocation(&_source,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column - 1,
                                           _column)};
                } else {
                    --_pos;
                    --_column;
                    return {LexemType::BitOr,
                            SourceLocation(&_source,
                                           getPrevPos(),
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column,
                                           _column)};
                }
            case '&':
                if (peek() == '&') {
                    return {LexemType::LogicAnd,
                            SourceLocation(&_source,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column - 1,
                                           _column)};
                } else {
                    --_pos;
                    --_column;
                    return {LexemType::BitAnd,
                            SourceLocation(&_source,
                                           getPrevPos(),
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column,
                                           _column)};
                }
            case '^':
                return {LexemType::BitXor,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case '<':
                if (peek() == '=') {
                    return {LexemType::LessEqual,
                            SourceLocation(&_source,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column - 1,
                                           _column)};
                } else {
                    --_pos;
                    --_column;
                    return {LexemType::Less,
                            SourceLocation(&_source,
                                           getPrevPos(),
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column,
                                           _column)};
                }
            case '>':
                if (peek() == '=') {
                    return {LexemType::GreaterEqual,
                            SourceLocation(&_source,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column - 1,
                                           _column)};
                } else {
                    --_pos;
                    --_column;
                    return {LexemType::Greater,
                            SourceLocation(&_source,
                                           getPrevPos(),
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column,
                                           _column)};
                }
            case '=':
                if (peek() == '=') {
                    return {LexemType::Equal,
                            SourceLocation(&_source,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column - 1,
                                           _column)};
                } else {
                    --_pos;
                    --_column;
                    return {LexemType::Assign,
                            SourceLocation(&_source,
                                           getPrevPos(),
                                           getPrevPos(),
                                           _line,
                                           _line,
                                           _column,
                                           _column)};
                }
            case ',':
                return {LexemType::Comma,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
            case ';':
                return {LexemType::Semicolon,
                        SourceLocation(&_source,
                                       getPrevPos(),
                                       getPrevPos(),
                                       _line,
                                       _line,
                                       _column,
                                       _column)};
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
                return {LexemType::EndOfFile, SourceLocation()};

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
            _line = 1;
            _column = 0;
            return;
        }
    }
}
void Lexer::revertTo(const Token& token) {
    _pos = token.getSourceLocation().getAbsoluteEnd() + 1;
    _line = token.getSourceLocation().getEndLine();
    _column = token.getSourceLocation().getEndColumn();
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
            SourceLocation(&_source,
                           startPos,
                           getPrevPos(),
                           startLine,
                           getPrevPos(),
                           startColumn,
                           getPrevPos()),
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
            _pos--;
            _column--;
            break;
        }

        string.push_back(c);
    }

    auto keyword = _keywords.find(string);

    if (keyword != _keywords.end()) {
        return {
            keyword->second,
            SourceLocation(&_source,
                           startPos,
                           getPrevPos(),
                           startLine,
                           _line,
                           startColumn,
                           _column),
        };
    } else {
        return {LexemType::Identifier,
                SourceLocation(&_source,
                               startPos,
                               getPrevPos(),
                               startLine,
                               _line,
                               startColumn,
                               _column),
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
            _pos--;
            _column--;
            break;
        }

        string.push_back(c);
    }

    return {
        LexemType::Number,
        SourceLocation(&_source, startPos, getPrevPos(), startLine, _line, startColumn, _column),
        std::move(string)};
}
