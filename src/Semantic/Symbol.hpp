// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_SYMBOL_HPP
#define VUG_SYMBOL_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "AST/ASTNodesForward.hpp"

class Type;

class Symbol {
   public:
    enum class Kind {
        Module,
        Type,
        Variable,
        Function,
    };
    enum class State {
        Placeholder,
        Incomplete,
        Complete,
    };

    Symbol(const Kind kind, std::string name)
        : mKind(kind),
          mState(State::Placeholder),
          mName(std::move(name)) {
    }

    Symbol(const Kind kind, const State state, std::string name)
        : mKind(kind),
          mState(state),
          mName(std::move(name)) {
    }

    [[nodiscard]] constexpr Kind getKind() const {
        return mKind;
    }
    [[nodiscard]] constexpr State getState() const {
        return mState;
    }
    [[nodiscard]] constexpr const std::string& getName() const {
        return mName;
    }

    constexpr void startDefinition() {
        mState = State::Incomplete;
    }
    constexpr void finishDefinition() {
        mState = State::Complete;
    }

   protected:
    const Kind mKind;
    State mState;
    std::string mName;
};

class ModuleSymbol : public Symbol {
   public:
    explicit ModuleSymbol(std::string name)
        : Symbol(Kind::Module, std::move(name)) {
    }

    std::vector<Symbol*> findMember(const std::string& name) const {
        const auto range = mMembers.equal_range(name);
        std::vector<Symbol*> result;

        for (auto iter = range.first; iter != range.second; ++iter) {
            result.push_back(iter->second);
        }

        return result;
    }
    void addMember(Symbol& symbol) {
        mMembers.insert(std::make_pair(symbol.getName(), &symbol));
    }

   protected:
    std::unordered_multimap<std::string, Symbol*> mMembers;
};

class TypeSymbol : public Symbol {
   public:
    explicit TypeSymbol(std::string name)
        : Symbol(Kind::Type, std::move(name)) {
    }

    [[nodiscard]] constexpr Type* getType() const {
        return mType;
    }
    constexpr void setType(Type* type) {
        mType = type;
    }

   protected:
    Type* mType{nullptr};
};

class LocalVariableSymbol : public Symbol {
   public:
    explicit LocalVariableSymbol(std::string name)
        : Symbol(Kind::Variable, std::move(name)) {
    }

    [[nodiscard]] constexpr TypeSymbol* getTypeSymbol() const {
        return mTypeSymbol;
    }
    constexpr void setTypeSymbol(TypeSymbol* type) {
        mTypeSymbol = type;
    }

   protected:
    TypeSymbol* mTypeSymbol{nullptr};
};

class FunctionSymbol : public Symbol {
   public:
    explicit FunctionSymbol(std::string name)
        : Symbol(Kind::Function, std::move(name)) {
    }

    [[nodiscard]] constexpr const std::vector<LocalVariableSymbol*>& getArguments() const {
        return mArguments;
    }
    constexpr void addArgument(LocalVariableSymbol& symbol) {
        mArguments.push_back(&symbol);
    }

    [[nodiscard]] constexpr StatementsBlock* getDefinition() const {
        return mDefinition;
    }
    constexpr void setDefinition(StatementsBlock& definition) {
        mDefinition = &definition;
    }

    [[nodiscard]] constexpr TypeSymbol* getTypeSymbol() const {
        return mTypeSymbol;
    }
    constexpr void setTypeSymbol(TypeSymbol* type) {
        mTypeSymbol = type;
    }

   protected:
    std::vector<LocalVariableSymbol*> mArguments;
    TypeSymbol* mTypeSymbol{nullptr};
    StatementsBlock* mDefinition{nullptr};
};
#endif  // VUG_SYMBOL_HPP
