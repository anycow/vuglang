// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_SYMBOLTABLE_HPP
#define VUG_SYMBOLTABLE_HPP

#include <cstddef>
#include <list>
#include <ostream>
#include <stack>
#include <string>
#include <unordered_map>

#include "Misc/Result.hpp"
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

    struct InsertError {
        enum class Kind { NameConflict, ProhibitedShadowing };

        const Kind kind;
        const SymbolTableRecord* conflictingSymbol;

        constexpr explicit InsertError(const Kind kind,
                                       const SymbolTableRecord* conflictingSymbol = nullptr)
            : kind(kind),
              conflictingSymbol(conflictingSymbol) {
        }

        friend std::ostream& operator<<(std::ostream& os, const Kind& kind) {
            switch (kind) {
                case Kind::NameConflict:
                    return os << "NameConflict";
                case Kind::ProhibitedShadowing:
                    return os << "ProhibitedShadowing";
                default:
                    return os << "Unknown";
            }
        }
        friend std::ostream& operator<<(std::ostream& os, const InsertError& obj) {
            return os << obj.kind;
        }
    };
    using InsertResult = Result<void, InsertError>;
    InsertResult insertSymbol(Symbol& symbol, bool canShadowed = true);

    enum class FindError { NotFound };
    using FindResult = Result<const SymbolTableRecord*, FindError>;
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
