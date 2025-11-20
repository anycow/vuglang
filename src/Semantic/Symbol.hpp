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

    Symbol(Kind kind, std::string name)
        : _kind(kind),
          _state(State::Placeholder),
          _name(std::move(name)) {
    }

    Symbol(Kind kind, State state, std::string name)
        : _kind(kind),
          _state(state),
          _name(std::move(name)) {
    }

    [[nodiscard]] Kind getKind() const noexcept {
        return _kind;
    }
    [[nodiscard]] State getState() const noexcept {
        return _state;
    }
    [[nodiscard]] const std::string& getName() const noexcept {
        return _name;
    }

    void startDefinition() {
        _state = State::Incomplete;
    }
    void finishDefinition() {
        _state = State::Complete;
    }

   protected:
    const Kind _kind;
    State _state;
    std::string _name;
};

class ModuleSymbol : public Symbol {
   public:
    ModuleSymbol(std::string name)
        : Symbol(Symbol::Kind::Module, std::move(name)),
          _members() {
    }

    std::vector<Symbol*> findMember(const std::string& name) const {
        auto range = _members.equal_range(name);
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
    TypeSymbol(std::string name)
        : Symbol(Symbol::Kind::Type, std::move(name)) {
    }

    [[nodiscard]] Type* getType() const {
        return _type;
    }
    void setType(Type* type) {
        _type = type;
    }

   protected:
    Type* _type{nullptr};
};

class LocalVariableSymbol : public Symbol {
   public:
    LocalVariableSymbol(std::string name)
        : Symbol(Symbol::Kind::Variable, std::move(name)) {
    }

    [[nodiscard]] TypeSymbol* getTypeSymbol() const {
        return _typeSymbol;
    }
    void setTypeSymbol(TypeSymbol* type) {
        _typeSymbol = type;
    }

   protected:
    TypeSymbol* _typeSymbol{nullptr};
};

class FunctionSymbol : public Symbol {
   public:
    FunctionSymbol(std::string name)
        : Symbol(Symbol::Kind::Function, std::move(name)),
          _arguments() {
    }

    [[nodiscard]] const std::vector<LocalVariableSymbol*>& getArguments() const {
        return _arguments;
    }
    void addArgument(LocalVariableSymbol& symbol) {
        _arguments.push_back(&symbol);
    }

    [[nodiscard]] StatementsBlock* getDefinition() const {
        return _definition;
    }
    void setDefinition(StatementsBlock& definition) {
        _definition = &definition;
    }

    [[nodiscard]] TypeSymbol* getTypeSymbol() const {
        return _typeSymbol;
    }
    void setTypeSymbol(TypeSymbol* type) {
        _typeSymbol = type;
    }

   protected:
    std::vector<LocalVariableSymbol*> _arguments;
    TypeSymbol* _typeSymbol{nullptr};
    StatementsBlock* _definition{nullptr};
};
#endif  // VUG_SYMBOL_HPP
