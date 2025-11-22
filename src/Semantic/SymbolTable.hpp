// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_SYMBOLTABLE_HPP
#define VUG_SYMBOLTABLE_HPP

#include <list>
#include <stack>
#include <unordered_map>

#include "Symbol.hpp"

struct SymbolTableRecord {
    Symbol& symbol;
    const size_t depth;
    const SymbolTableRecord* shadowedRecord{nullptr};
    const bool canShadowed;
    // SymbolTableRecord* overridedRecord{nullptr};

    constexpr explicit SymbolTableRecord(Symbol& symbol, const size_t depth, const bool canShadowed)
        : symbol(symbol),
          depth(depth),
          canShadowed(canShadowed) {
    }
};

class SymbolTable {
   public:
    SymbolTable() = default;

    struct InsertResult {
        enum class Kind { Successful, NameConflict, ProhibitedShadowing };

        const Kind kind;
        const SymbolTableRecord* conflictingSymbol;

        constexpr explicit InsertResult(const Kind kind, const SymbolTableRecord* conflictingSymbol = nullptr)
            : kind(kind),
              conflictingSymbol(conflictingSymbol) {
        }
    };
    InsertResult insertSymbol(Symbol& symbol, bool canShadowed = true);

    struct FindResult {
        enum class Kind { Successful, NotFound };

        const Kind kind;
        const SymbolTableRecord* record;

        constexpr explicit FindResult(const Kind kind, const SymbolTableRecord* record = nullptr)
            : kind(kind),
              record(record) {
        }
    };
    FindResult findSymbol(const std::string& name);

    [[nodiscard]] size_t getDepth() const {
        return mScopes.size();
    }
    size_t openScope() {
        mScopes.emplace();

        return getDepth();
    }
    size_t closeScope() {
        for (auto& record : mScopes.top()) {
            if (record.shadowedRecord) {
                mNames[record.symbol.getName()] = record.shadowedRecord;
            } else {
                mNames.erase(record.symbol.getName());
            }
        }
        mScopes.pop();

        return getDepth();
    }

   private:
    std::stack<std::list<SymbolTableRecord>, std::list<std::list<SymbolTableRecord>>> mScopes;
    std::unordered_map<std::string, const SymbolTableRecord*> mNames;
};

#endif  // VUG_SYMBOLTABLE_HPP
