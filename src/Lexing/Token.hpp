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

class Token {
private:
    LexemType _type;
    std::string _value;

public:
    Token(LexemType type = LexemType::EndOfFile,
          const std::string&& value = "") : _type(type), _value(value) {}


    [[nodiscard]] LexemType getType() const {
        return _type;
    }

    [[nodiscard]] const std::string& getValue() const {
        return _value;
    }

    [[nodiscard]] inline std::string toString() const {
        return "Type: " + TokenTypeNames[_type] + " Value: " + (!_value.empty() ? _value : "Empty");
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
};

#endif// VUG_TOKEN_HPP
