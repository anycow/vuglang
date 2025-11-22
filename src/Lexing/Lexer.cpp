// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "Lexer.hpp"
#include <stdexcept>
#include "Token.hpp"

Token Lexer::getToken() {
    while (true) {
        switch (const auto c = peek()) {
            case '(':
                return {LexemType::LeftRoundBracket,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case ')':
                return {LexemType::RightRoundBracket,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case '{':
                return {LexemType::LeftCurlyBracket,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case '}':
                return {LexemType::RightCurlyBracket,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case '+':
                return {LexemType::Plus,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case '-':
                if (peek() == '>') {
                    return {LexemType::Arrow,
                            SourceLocation(&mSource,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn - 1,
                                           mColumn)};
                } else {
                    --mPos;
                    --mColumn;
                    return {LexemType::Minus,
                            SourceLocation(&mSource,
                                           getPrevPos(),
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn,
                                           mColumn)};
                }
            case '*':
                return {LexemType::Multiply,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case '/':
                return {LexemType::Divide,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case '%':
                return {LexemType::Remainder,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case '!':
                if (peek() == '=') {
                    return {LexemType::Unequal,
                            SourceLocation(&mSource,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn - 1,
                                           mColumn)};
                } else {
                    --mPos;
                    --mColumn;
                    return {LexemType::Not,
                            SourceLocation(&mSource,
                                           getPrevPos(),
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn,
                                           mColumn)};
                }
            case '|':
                if (peek() == '|') {
                    return {LexemType::LogicOr,
                            SourceLocation(&mSource,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn - 1,
                                           mColumn)};
                } else {
                    --mPos;
                    --mColumn;
                    return {LexemType::BitOr,
                            SourceLocation(&mSource,
                                           getPrevPos(),
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn,
                                           mColumn)};
                }
            case '&':
                if (peek() == '&') {
                    return {LexemType::LogicAnd,
                            SourceLocation(&mSource,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn - 1,
                                           mColumn)};
                } else {
                    --mPos;
                    --mColumn;
                    return {LexemType::BitAnd,
                            SourceLocation(&mSource,
                                           getPrevPos(),
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn,
                                           mColumn)};
                }
            case '^':
                return {LexemType::BitXor,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case '<':
                if (peek() == '=') {
                    return {LexemType::LessEqual,
                            SourceLocation(&mSource,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn - 1,
                                           mColumn)};
                } else {
                    --mPos;
                    --mColumn;
                    return {LexemType::Less,
                            SourceLocation(&mSource,
                                           getPrevPos(),
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn,
                                           mColumn)};
                }
            case '>':
                if (peek() == '=') {
                    return {LexemType::GreaterEqual,
                            SourceLocation(&mSource,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn - 1,
                                           mColumn)};
                } else {
                    --mPos;
                    --mColumn;
                    return {LexemType::Greater,
                            SourceLocation(&mSource,
                                           getPrevPos(),
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn,
                                           mColumn)};
                }
            case '=':
                if (peek() == '=') {
                    return {LexemType::Equal,
                            SourceLocation(&mSource,
                                           getPrevPos() - 1,
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn - 1,
                                           mColumn)};
                } else {
                    --mPos;
                    --mColumn;
                    return {LexemType::Assign,
                            SourceLocation(&mSource,
                                           getPrevPos(),
                                           getPrevPos(),
                                           mLine,
                                           mLine,
                                           mColumn,
                                           mColumn)};
                }
            case ',':
                return {LexemType::Comma,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case ';':
                return {LexemType::Semicolon,
                        SourceLocation(&mSource,
                                       getPrevPos(),
                                       getPrevPos(),
                                       mLine,
                                       mLine,
                                       mColumn,
                                       mColumn)};
            case '"':
                return getString();

            case ' ':
            case '\r':
                break;
            case '\t':
                mColumn += 4;
                break;
            case '\n':
                mColumn = 0;
                ++mLine;
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
            mPos = 0;
            mLine = 1;
            mColumn = 0;
            return;
        }
    }
}
void Lexer::revertTo(const Token& token) {
    mPos = token.getSourceLocation().getAbsoluteEnd() + 1;
    mLine = token.getSourceLocation().getEndLine();
    mColumn = token.getSourceLocation().getEndColumn();
}
Token Lexer::getString() {
    std::string string;

    const auto startPos = getPrevPos();
    const auto startLine = mLine;
    const auto startColumn = mColumn;

    while (true) {
        const char c = peek();

        if (c == '\0') {
            throw std::logic_error("Bad lexing");
        }
        if (c == '\r') {
            continue;
        }
        if (c == '\n') {
            mColumn = 0;
            ++mLine;
        }

        if (c == '"') {
            break;
        }

        string.push_back(c);
    }
    return {LexemType::String,
            SourceLocation(&mSource,
                           startPos,
                           getPrevPos(),
                           startLine,
                           getPrevPos(),
                           startColumn,
                           getPrevPos()),
            std::move(string)};
}
Token Lexer::getIdentifier(const char firstChar) {
    const auto startPos = getPrevPos();
    const auto startLine = mLine;
    const auto startColumn = mColumn;

    std::string string;
    string.push_back(firstChar);

    while (true) {
        const char c = peek();

        if (!isalpha(c) && !isdigit(c)) {
            mPos--;
            mColumn--;
            break;
        }

        string.push_back(c);
    }

    const auto keyword = mKeywords.find(string);

    if (keyword != mKeywords.end()) {
        return {
            keyword->second,
            SourceLocation(&mSource,
                           startPos,
                           getPrevPos(),
                           startLine,
                           mLine,
                           startColumn,
                           mColumn),
        };
    } else {
        return {LexemType::Identifier,
                SourceLocation(&mSource,
                               startPos,
                               getPrevPos(),
                               startLine,
                               mLine,
                               startColumn,
                               mColumn),
                std::move(string)};
    }
}
Token Lexer::getNumber(const char firstChar) {
    std::string string;
    string.push_back(firstChar);

    const auto startPos = getPrevPos();
    const auto startLine = mLine;
    const auto startColumn = mColumn;

    while (true) {
        const char c = peek();

        if (!isdigit(c)) {
            mPos--;
            mColumn--;
            break;
        }

        string.push_back(c);
    }

    return {
        LexemType::Number,
        SourceLocation(&mSource, startPos, getPrevPos(), startLine, mLine, startColumn, mColumn),
        std::move(string)};
}
