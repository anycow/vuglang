// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_TOKEN_HPP
#define VUG_TOKEN_HPP

#include <cstdint>
#include <string>
#include <unordered_map>

enum class [[maybe_unused]] LexemType : uint32_t {
    Identifier,
    Number,
    String,

    // KEYWORDS
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

    // OPERATORS
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
    constexpr SourceLocation(const SourceFile* sourceFile,
                             const int64_t absoluteStart,
                             const int64_t absoluteEnd,
                             const int64_t startLine,
                             const int64_t endLine,
                             const int64_t startColumn,
                             const int64_t endColumn)
        : mSourceFile(sourceFile),
          mAbsoluteStart(absoluteStart),
          mAbsoluteEnd(absoluteEnd),
          mStartLine(startLine),
          mEndLine(endLine),
          mStartColumn(startColumn),
          mEndColumn(endColumn),
          mIsValid(true) {
    }

    constexpr SourceLocation(const SourceLocation& start, const SourceLocation& end)
        : mSourceFile(start.mSourceFile),
          mAbsoluteStart(start.mAbsoluteStart),
          mAbsoluteEnd(end.mAbsoluteEnd),
          mStartLine(start.mStartLine),
          mEndLine(end.mEndLine),
          mStartColumn(start.mStartColumn),
          mEndColumn(end.mEndColumn),
          mIsValid(start.isValid() && end.isValid()) {
    }

    constexpr SourceLocation()
        : mSourceFile(nullptr),
          mAbsoluteStart(-1),
          mAbsoluteEnd(-1),
          mStartLine(-1),
          mEndLine(-1),
          mStartColumn(-1),
          mEndColumn(-1),
          mIsValid(false) {
    }

    [[nodiscard]] constexpr const SourceFile* getSourceFile() const {
        return mSourceFile;
    }
    [[nodiscard]] constexpr int64_t getAbsoluteStart() const {
        return mAbsoluteStart;
    }
    [[nodiscard]] constexpr int64_t getAbsoluteEnd() const {
        return mAbsoluteEnd;
    }
    [[nodiscard]] constexpr int64_t getStartLine() const {
        return mStartLine;
    }
    [[nodiscard]] constexpr int64_t getEndLine() const {
        return mEndLine;
    }
    [[nodiscard]] constexpr int64_t getStartColumn() const {
        return mStartColumn;
    }
    [[nodiscard]] constexpr int64_t getEndColumn() const {
        return mEndColumn;
    }
    [[nodiscard]] constexpr bool isValid() const {
        return mIsValid;
    }

    [[nodiscard]] std::string toString() const {
        if (mIsValid) {
            return std::to_string(mAbsoluteStart) + ", " + std::to_string(mAbsoluteEnd) + ", "
                   + std::to_string(mStartLine) + ", " + std::to_string(mEndLine) + ", "
                   + std::to_string(mStartColumn) + ", " + std::to_string(mEndColumn);
        } else {
            return "Invalid location";
        }
    }

   private:
    const SourceFile* mSourceFile;
    int64_t mAbsoluteStart;
    int64_t mAbsoluteEnd;
    int64_t mStartLine;
    int64_t mEndLine;
    int64_t mStartColumn;
    int64_t mEndColumn;
    bool mIsValid;
};

class Token {
   public:
    constexpr Token(const LexemType type,
                    const SourceLocation& sourceLocation,
                    const std::string&& value = "")
        : mType(type),
          mSourceLocation(sourceLocation),
          mValue(value) {
    }

    [[nodiscard]] constexpr LexemType getType() const {
        return mType;
    }

    [[nodiscard]] constexpr const SourceLocation& getSourceLocation() const {
        return mSourceLocation;
    }

    [[nodiscard]] constexpr const std::string& getValue() const {
        return mValue;
    }

    [[nodiscard]] std::string toString() const {
        return "Type: " + TokenTypeNames[mType] + " Value: " + (!mValue.empty() ? mValue : "Empty")
               + " Location: (" + mSourceLocation.toString() + ")";
    }

    bool constexpr operator==(const LexemType& type) const {
        return mType == type;
    }
    bool constexpr operator!=(const LexemType& type) const {
        return mType != type;
    }

    bool constexpr operator==(const Token& rhs) const {
        return mType == rhs.mType && mValue == rhs.mValue;
    }
    bool constexpr operator!=(const Token& rhs) const {
        return !(rhs == *this);
    }

   protected:
    LexemType mType;
    SourceLocation mSourceLocation;
    std::string mValue;
};

#endif  // VUG_TOKEN_HPP
