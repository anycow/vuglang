// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "SymbolContext.hpp"

#include "Semantic/SymbolTable.hpp"
#include "Semantic/Types/BooleanType.hpp"
#include "Semantic/Types/IntegerType.hpp"

SymbolContext::SymbolContext(SymbolTable& table)
    : mSymbolTable(table) {
    mSymbolTable.openScope();

    const auto bits = {8, 16, 32, 64};
    for (const auto& size : bits) {
        auto signedInt = std::make_unique<TypeSymbol>("int" + std::to_string(size));
        auto unsignedInt = std::make_unique<TypeSymbol>("uint" + std::to_string(size));

        mSymbolTable.insertSymbol(*signedInt, false);
        mSymbolTable.insertSymbol(*unsignedInt, false);

        signedInt->startDefinition();
        mType.push_back(
            std::make_unique<IntegerType>(*this, "int" + std::to_string(size), size, true));
        signedInt->setType(mType.back().get());
        signedInt->finishDefinition();

        unsignedInt->startDefinition();
        mType.push_back(
            std::make_unique<IntegerType>(*this, "uint" + std::to_string(size), size, false));
        unsignedInt->setType(mType.back().get());
        unsignedInt->finishDefinition();

        mSignedIntegerTypeSymbols.insert({size, signedInt.get()});
        mUnsignedIntegerTypeSymbols.insert({size, unsignedInt.get()});

        mSymbols.push_back(std::move(signedInt));
        mSymbols.push_back(std::move(unsignedInt));
    }

    mType.push_back(std::make_unique<BooleanType>(*this, "bool"));

    auto boolean = std::make_unique<TypeSymbol>("bool");
    mSymbolTable.insertSymbol(*boolean, false);

    boolean->startDefinition();
    boolean->setType(mType.back().get());
    boolean->finishDefinition();

    mBooleanTypeSymbol = boolean.get();
    mSymbols.emplace_back(std::move(boolean));
}
