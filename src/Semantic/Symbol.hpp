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
        : kind_(kind),
          state_(State::Placeholder),
          _name(std::move(name)) {
    }

    Symbol(const Kind kind, const State state, std::string name)
        : kind_(kind),
          state_(state),
          _name(std::move(name)) {
    }

    [[nodiscard]] constexpr Kind getKind() const {
        return kind_;
    }
    [[nodiscard]] constexpr State getState() const {
        return state_;
    }
    [[nodiscard]] constexpr const std::string& getName() const {
        return _name;
    }

    constexpr void startDefinition() {
        state_ = State::Incomplete;
    }
    constexpr void finishDefinition() {
        state_ = State::Complete;
    }

   protected:
    const Kind kind_;
    State state_;
    std::string _name;
};

class ModuleSymbol : public Symbol {
   public:
    explicit ModuleSymbol(std::string name)
        : Symbol(Kind::Module, std::move(name)) {
    }

    std::vector<Symbol*> findMember(const std::string& name) const {
        const auto range = _members.equal_range(name);
        std::vector<Symbol*> result;

        for (auto iter = range.first; iter != range.second; ++iter) {
            result.push_back(iter->second);
        }

        return result;
    }
    void addMember(Symbol& symbol) {
        _members.insert(std::make_pair(symbol.getName(), &symbol));
    }

   protected:
    std::unordered_multimap<std::string, Symbol*> _members;
};

class TypeSymbol : public Symbol {
   public:
    explicit TypeSymbol(std::string name)
        : Symbol(Kind::Type, std::move(name)) {
    }

    [[nodiscard]] constexpr Type* getType() const {
        return _type;
    }
    constexpr void setType(Type* type) {
        _type = type;
    }

   protected:
    Type* _type{nullptr};
};

class LocalVariableSymbol : public Symbol {
   public:
    explicit LocalVariableSymbol(std::string name)
        : Symbol(Kind::Variable, std::move(name)) {
    }

    [[nodiscard]] constexpr TypeSymbol* getTypeSymbol() const {
        return _typeSymbol;
    }
    constexpr void setTypeSymbol(TypeSymbol* type) {
        _typeSymbol = type;
    }

   protected:
    TypeSymbol* _typeSymbol{nullptr};
};

class FunctionSymbol : public Symbol {
   public:
    explicit FunctionSymbol(std::string name)
        : Symbol(Kind::Function, std::move(name)) {
    }

    [[nodiscard]] constexpr const std::vector<LocalVariableSymbol*>& getArguments() const {
        return _arguments;
    }
    constexpr void addArgument(LocalVariableSymbol& symbol) {
        _arguments.push_back(&symbol);
    }

    [[nodiscard]] constexpr StatementsBlock* getDefinition() const {
        return _definition;
    }
    constexpr void setDefinition(StatementsBlock& definition) {
        _definition = &definition;
    }

    [[nodiscard]] constexpr TypeSymbol* getTypeSymbol() const {
        return _typeSymbol;
    }
    constexpr void setTypeSymbol(TypeSymbol* type) {
        _typeSymbol = type;
    }

   protected:
    std::vector<LocalVariableSymbol*> _arguments;
    TypeSymbol* _typeSymbol{nullptr};
    StatementsBlock* _definition{nullptr};
};
#endif  // VUG_SYMBOL_HPP
