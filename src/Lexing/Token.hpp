// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_TOKEN_HPP
#define VUG_TOKEN_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

enum class [[maybe_unused]] LexemType : uint32_t {
    Identifier,
    Number,
    String,

    //KEYWORDS
    Mod,
    Var,
    Func,
    Type,
    Decl,
    If,
    Else,
    While,
    Break,
    Return,

    //OPERATORS
    Plus,
    Minus,
    Multiply,
    Divide,
    Remainder,
    Not,
    LogicOr,
    LogicAnd,
    BitOr,
    BitXor,
    BitAnd,
    Assign,
    Equal,
    Unequal,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    Arrow,

    LeftRoundBracket,
    RightRoundBracket,

    LeftSquareBracket,
    RightSquareBracket,

    LeftCurlyBracket,
    RightCurlyBracket,

    Comma,
    Semicolon,

    EndOfFile,
};

inline std::unordered_map<LexemType, std::string> TokenTypeNames = {
        {LexemType::Identifier, "Identifier"},
        {LexemType::Number, "Number"},
        {LexemType::String, "String"},
        {LexemType::Mod, "Module"},
        {LexemType::Var, "Var"},
        {LexemType::Func, "Function"},
        {LexemType::Type, "Type"},
        {LexemType::Decl, "Declare"},
        {LexemType::While, "While"},
        {LexemType::If, "If"},
        {LexemType::Else, "Else"},
        {LexemType::Plus, "+"},
        {LexemType::Minus, "-"},
        {LexemType::Multiply, "*"},
        {LexemType::Divide, "/"},
        {LexemType::Remainder, "%"},
        {LexemType::Not, "!"},
        {LexemType::LogicOr, "||"},
        {LexemType::LogicAnd, "&&"},
        {LexemType::BitOr, "|"},
        {LexemType::BitXor, "^"},
        {LexemType::BitAnd, "&"},
        {LexemType::Assign, "="},
        {LexemType::Equal, "=="},
        {LexemType::Unequal, "!="},
        {LexemType::Less, "<"},
        {LexemType::LessEqual, "<="},
        {LexemType::Greater, ">"},
        {LexemType::GreaterEqual, ">="},
        {LexemType::Arrow, "->"},
        {LexemType::LeftRoundBracket, "("},
        {LexemType::RightRoundBracket, ")"},
        {LexemType::LeftCurlyBracket, "{"},
        {LexemType::RightCurlyBracket, "}"},
        {LexemType::Comma, "Comma"},
        {LexemType::Semicolon, "Semicolon"},
        {LexemType::EndOfFile, "EOF"},
};

class SourceLocation {
public:
    SourceLocation(size_t absoluteStart,
                   size_t absoluteEnd,
                   size_t startLine,
                   size_t endLine,
                   size_t startColumn,
                   size_t endColumn)
        : _absoluteStart(absoluteStart),
          _absoluteEnd(absoluteEnd),
          _startLine(startLine),
          _endLine(endLine),
          _startColumn(startColumn),
          _endColumn(endColumn),
          _isValid(true) {}

    SourceLocation()
        : _absoluteStart(-1),
          _absoluteEnd(-1),
          _startLine(-1),
          _endLine(-1),
          _startColumn(-1),
          _endColumn(-1),
          _isValid(false) {}

    [[nodiscard]] inline size_t getAbsoluteStart() const {
        return _absoluteStart;
    }
    [[nodiscard]] inline size_t getAbsoluteEnd() const {
        return _absoluteEnd;
    }
    [[nodiscard]] inline size_t getStartLine() const {
        return _startLine;
    }
    [[nodiscard]] inline size_t getEndLine() const {
        return _endLine;
    }
    [[nodiscard]] inline size_t getStartColumn() const {
        return _startColumn;
    }
    [[nodiscard]] inline size_t getEndColumn() const {
        return _endColumn;
    }
    [[nodiscard]] inline bool isValid() const {
        return _isValid;
    }

    [[nodiscard]] inline std::string toString() const {
        if (_isValid) {
            return std::to_string(_absoluteStart) + ", " +
                   std::to_string(_absoluteEnd) + ", " +
                   std::to_string(_startLine) + ", " +
                   std::to_string(_endLine) + ", " +
                   std::to_string(_startColumn) + ", " +
                   std::to_string(_endColumn);
        } else {
            return "Invalid location";
        }
    }

protected:
    size_t _absoluteStart;
    size_t _absoluteEnd;
    size_t _startLine;
    size_t _endLine;
    size_t _startColumn;
    size_t _endColumn;
    bool _isValid;
};

class Token {
public:
    Token(LexemType type,
          SourceLocation sourceLocation,
          const std::string&& value = "")
        : _type(type),
          _sourceLocation(sourceLocation),
          _value(value) {}

    [[nodiscard]] LexemType getType() const {
        return _type;
    }

    [[nodiscard]] const std::string& getValue() const {
        return _value;
    }

    [[nodiscard]] inline std::string toString() const {
        return "Type: " + TokenTypeNames[_type] +
               " Value: " + (!_value.empty() ? _value : "Empty") +
               " Location: (" + _sourceLocation.toString() + ")";
    }

    bool operator==(const LexemType& type) const {
        return _type == type;
    }
    bool operator!=(const LexemType& type) const {
        return _type != type;
    }

    bool operator==(const Token& rhs) const {
        return _type == rhs._type &&
               _value == rhs._value;
    }
    bool operator!=(const Token& rhs) const {
        return !(rhs == *this);
    }

protected:
    LexemType _type;
    SourceLocation _sourceLocation;
    std::string _value;
};

#endif// VUG_TOKEN_HPP
