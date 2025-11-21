// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "SymbolTable.hpp"

SymbolTable::InsertResult SymbolTable::insertSymbol(Symbol& symbol, const bool canShadowed) {
    auto& record = _scopes.top().emplace_back(symbol, getDepth(), canShadowed);
    const auto it = _names.find(record.symbol.getName());

    if (it == _names.end()) {
        _names.insert({record.symbol.getName(), &record});
    } else {
        auto shadowedRecord = it->second;

        if (shadowedRecord->depth != record.depth) {
            if (shadowedRecord->canShadowed) {
                _names[record.symbol.getName()] = &record;
                record.shadowedRecord = shadowedRecord;
            } else {
                return InsertResult(InsertResult::Kind::ProhibitedShadowing, shadowedRecord);
            }
        } else {
            return InsertResult(InsertResult::Kind::NameConflict, shadowedRecord);
        }
    }

    return InsertResult(InsertResult::Kind::Successful);
}
SymbolTable::FindResult SymbolTable::findSymbol(const std::string& name) {
    const auto it = _names.find(name);

    if (it == _names.end()) {
        return FindResult(FindResult::Kind::NotFound);
    } else {
        return FindResult(FindResult::Kind::Successful, it->second);
    }
}
