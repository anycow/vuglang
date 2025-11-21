// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "SymbolContext.hpp"

#include "Semantic/SymbolTable.hpp"

SymbolContext::SymbolContext(SymbolTable& table)
    : _symbolTable(table) {
    _symbolTable.openScope();

    const auto bits = {8, 16, 32, 64};
    for (const auto& size : bits) {
        auto signedInt = std::make_unique<TypeSymbol>("int" + std::to_string(size));
        auto unsignedInt = std::make_unique<TypeSymbol>("uint" + std::to_string(size));

        _symbolTable.insertSymbol(*signedInt, false);
        _symbolTable.insertSymbol(*unsignedInt, false);

        signedInt->startDefinition();
        _type.push_back(
            std::make_unique<IntegerType>(*this, "int" + std::to_string(size), size, true));
        signedInt->setType(_type.back().get());
        signedInt->finishDefinition();

        unsignedInt->startDefinition();
        _type.push_back(
            std::make_unique<IntegerType>(*this, "uint" + std::to_string(size), size, false));
        unsignedInt->setType(_type.back().get());
        unsignedInt->finishDefinition();

        _signedIntegerTypeSymbols.insert({size, signedInt.get()});
        _unsignedIntegerTypeSymbols.insert({size, unsignedInt.get()});

        _symbols.push_back(std::move(signedInt));
        _symbols.push_back(std::move(unsignedInt));
    }

    _type.push_back(std::make_unique<BooleanType>(*this, "bool"));

    auto boolean = std::make_unique<TypeSymbol>("bool");
    _symbolTable.insertSymbol(*boolean, false);

    boolean->startDefinition();
    boolean->setType(_type.back().get());
    boolean->finishDefinition();

    _booleanTypeSymbol = boolean.get();
    _symbols.emplace_back(std::move(boolean));
}
