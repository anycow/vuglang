// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_LEXER_HPP
#define VUG_LEXER_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "Diagnostic/Logger.hpp"
#include "Misc/SourceManager.hpp"
#include "Token.hpp"

class Lexer {
   public:
    explicit Lexer(const SourceFile& source)
        : mKeywords({
              {"mod", LexemType::Mod},
              {"func", LexemType::Func},
              {"var", LexemType::Var},
              {"if", LexemType::If},
              {"else", LexemType::Else},
              {"while", LexemType::While},
              {"break", LexemType::Break},
              {"return", LexemType::Return},
          }),
          mSource(source) {
    }

    bool match(const Token& token) {
        return getToken() == token;
    }

    Token getToken();
    void getTokens(std::vector<Token>& tokens);

    void revertTo(const Token& token);

   protected:
    char peek() {
        ++mPos;
        ++mColumn;
        if (mPos <= static_cast<int64_t>(mSource.getText().size())) {
            return mSource.getText()[mPos - 1];
        }
        return '\0';
    }
    [[nodiscard]] char peekCurrent() const {
        if (mPos <= static_cast<int64_t>(mSource.getText().size())) {
            return mSource.getText()[mPos - 1];
        }
        return '\0';
    }
    inline int64_t getPrevPos() const {
        return mPos - 1;
    }

    Token getString();
    Token getIdentifier(char firstChar);
    Token getNumber(char firstChar);

   private:
    std::unordered_map<std::string, LexemType> mKeywords;
    int64_t mPos{0};
    int64_t mLine{1};
    int64_t mColumn{0};
    const SourceFile& mSource;
};


#endif  // VUG_LEXER_HPP
