// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_SYMBOLTABLE_HPP
#define VUG_SYMBOLTABLE_HPP

#include <cstdint>
#include <list>
#include <stack>
#include <unordered_map>

#include "Symbol.hpp"

struct SymbolTableRecord {
    Symbol& symbol;
    const size_t depth;
    const SymbolTableRecord* shadowedRecord{nullptr};
    const bool canShadowed;
    //SymbolTableRecord* overridedRecord{nullptr};

    explicit SymbolTableRecord(Symbol& symbol, size_t depth, bool canShadowed)
        : symbol(symbol),
          depth(depth),
          canShadowed(canShadowed) {}
};

class SymbolTable {
public:
    SymbolTable() = default;

    struct InsertResult {
        enum class Kind {
            Successful,
            NameConflict,
            ProhibitedShadowing
        };

        const Kind kind;
        const SymbolTableRecord* conflictingSymbol;

        InsertResult(Kind kind, const SymbolTableRecord* conflictingSymbol = nullptr)
            : kind(kind),
              conflictingSymbol(conflictingSymbol) {}
    };
    InsertResult insertSymbol(Symbol& symbol, bool canShadowed = true);

    struct FindResult {
        enum class Kind {
            Successful,
            NotFound
        };

        const Kind kind;
        const SymbolTableRecord* record;

        FindResult(Kind kind, const SymbolTableRecord* record = nullptr)
            : kind(kind),
              record(record) {}
    };
    FindResult findSymbol(const std::string& name);

    [[nodiscard]] inline size_t getDepth() const {
        return _scopes.size();
    }
    inline size_t openScope() {
        _scopes.emplace();

        return getDepth();
    }
    inline size_t closeScope() {
        for (auto& record: _scopes.top()) {
            if (record.shadowedRecord) {
                _names[record.symbol.getName()] = record.shadowedRecord;
            } else {
                _names.erase(record.symbol.getName());
            }
        }
        _scopes.pop();

        return getDepth();
    }

protected:
    std::stack<std::list<SymbolTableRecord>,
               std::list<std::list<SymbolTableRecord>>>
            _scopes;
    std::unordered_map<std::string, const SymbolTableRecord*> _names;
};

#endif//VUG_SYMBOLTABLE_HPP
