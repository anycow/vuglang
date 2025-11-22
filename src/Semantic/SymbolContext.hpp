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
    explicit SymbolContext(SymbolTable& table);

    template <typename T = Symbol, typename... Args>
    constexpr T* addSymbol(Args&&... args) {
        mSymbols.push_back(std::move(std::make_unique<T>(std::forward<Args>(args)...)));
        return static_cast<T*>(mSymbols.back().get());
    }

    [[nodiscard]] constexpr SymbolTable& getSymbolTable() const {
        return mSymbolTable;
    }

    [[nodiscard]] const TypeSymbol* getIntType(const uint32_t bits, const bool isSigned) const {
        if (isSigned) {
            return mSignedIntegerTypeSymbols.find(bits)->second;
        } else {
            return mUnsignedIntegerTypeSymbols.find(bits)->second;
        }
    }
    [[nodiscard]] constexpr const TypeSymbol* getBoolType() const {
        return mBooleanTypeSymbol;
    }

   private:
    std::vector<std::unique_ptr<Symbol>> mSymbols;
    std::vector<std::unique_ptr<Type>> mType;
    SymbolTable& mSymbolTable;

    std::unordered_map<uint32_t, const TypeSymbol*> mSignedIntegerTypeSymbols;
    std::unordered_map<uint32_t, const TypeSymbol*> mUnsignedIntegerTypeSymbols;
    const TypeSymbol* mBooleanTypeSymbol;
};
#endif  // VUG_SYMBOLCONTEXT_HPP
