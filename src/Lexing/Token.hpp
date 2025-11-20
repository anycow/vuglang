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

class SourceFile;
class SourceLocation {
public:
    SourceLocation(const SourceFile* sourceFile,
                   int64_t absoluteStart,
                   int64_t absoluteEnd,
                   int64_t startLine,
                   int64_t endLine,
                   int64_t startColumn,
                   int64_t endColumn)
        : _sourceFile(sourceFile),
          _absoluteStart(absoluteStart),
          _absoluteEnd(absoluteEnd),
          _startLine(startLine),
          _endLine(endLine),
          _startColumn(startColumn),
          _endColumn(endColumn),
          _isValid(true) {}

    SourceLocation(const SourceLocation& start,
                   const SourceLocation& end)
        : _sourceFile(start._sourceFile),
          _absoluteStart(start._absoluteStart),
          _absoluteEnd(end._absoluteEnd),
          _startLine(start._startLine),
          _endLine(end._endLine),
          _startColumn(start._startColumn),
          _endColumn(end._endColumn),
          _isValid(start.isValid() && end.isValid()) {}

    explicit SourceLocation()
        : _sourceFile(nullptr),
          _absoluteStart(-1),
          _absoluteEnd(-1),
          _startLine(-1),
          _endLine(-1),
          _startColumn(-1),
          _endColumn(-1),
          _isValid(false) {}

    [[nodiscard]] inline const SourceFile* getSourceFile() const {
        return _sourceFile;
    }
    [[nodiscard]] inline int64_t getAbsoluteStart() const {
        return _absoluteStart;
    }
    [[nodiscard]] inline int64_t getAbsoluteEnd() const {
        return _absoluteEnd;
    }
    [[nodiscard]] inline int64_t getStartLine() const {
        return _startLine;
    }
    [[nodiscard]] inline int64_t getEndLine() const {
        return _endLine;
    }
    [[nodiscard]] inline int64_t getStartColumn() const {
        return _startColumn;
    }
    [[nodiscard]] inline int64_t getEndColumn() const {
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

private:
    const SourceFile* _sourceFile;
    int64_t _absoluteStart;
    int64_t _absoluteEnd;
    int64_t _startLine;
    int64_t _endLine;
    int64_t _startColumn;
    int64_t _endColumn;
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

    [[nodiscard]] const SourceLocation& getSourceLocation() const {
        return _sourceLocation;
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
