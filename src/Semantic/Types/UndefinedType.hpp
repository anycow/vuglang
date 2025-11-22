// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_UNDEFINED_TYPE_HPP
#define VUG_UNDEFINED_TYPE_HPP

#include "Type.hpp"

class SymbolContext;

class UndefinedType : public Type {
   public:
    constexpr explicit UndefinedType(SymbolContext& context)
        : Type(context, "Undefined", TypeKind::Undefined) {
    }

    [[nodiscard]] OperationResultType binaryOperationType(LexemType opType,
                                                          const Type& rhs) const override;
    [[nodiscard]] OperationResultType prefixOperationType(LexemType opType) const override;

    [[nodiscard]] bool isUndefined() const override;
};

#endif  // VUG_UNDEFINED_TYPE_HPP

