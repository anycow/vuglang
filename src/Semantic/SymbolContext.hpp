// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_SYMBOLCONTEXT_HPP
#define VUG_SYMBOLCONTEXT_HPP

#include <string>

#include "Semantic/Symbol.hpp"
#include "Semantic/Type.hpp"

class SymbolTable;

class SymbolContext {
   public:
    SymbolContext(SymbolTable& table);

    template <typename T = Symbol, typename... Args>
    inline T* addSymbol(Args&&... args) {
        _symbols.push_back(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
        return static_cast<T*>(_symbols.back().get());
    }

    [[nodiscard]] inline SymbolTable& getSymbolTable() const {
        return _symbolTable;
    }

    [[nodiscard]] inline const TypeSymbol* getIntType(uint32_t bits, bool isSigned) const {
        if (isSigned) {
            return _signedIntegerTypeSymbols.find(bits)->second;
        } else {
            return _unsignedIntegerTypeSymbols.find(bits)->second;
        }
    }
    [[nodiscard]] inline const TypeSymbol* getBoolType() const {
        return _booleanTypeSymbol;
    }

   protected:
    std::vector<std::unique_ptr<Symbol>> _symbols;
    std::vector<std::unique_ptr<Type>> _type;
    SymbolTable& _symbolTable;

    std::unordered_map<uint32_t, const TypeSymbol*> _signedIntegerTypeSymbols;
    std::unordered_map<uint32_t, const TypeSymbol*> _unsignedIntegerTypeSymbols;
    const TypeSymbol* _booleanTypeSymbol;
};
#endif  // VUG_SYMBOLCONTEXT_HPP
