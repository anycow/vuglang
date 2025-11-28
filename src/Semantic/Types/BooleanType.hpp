// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_BOOLEAN_TYPE_HPP
#define VUG_BOOLEAN_TYPE_HPP

#include "PrimitiveType.hpp"

class SymbolContext;

class BooleanType : public PrimitiveType {
   public:
    constexpr BooleanType(SymbolContext& context, std::string typeName)
        : PrimitiveType(context, std::move(typeName), PrimitiveKind::Boolean) {
    }

    [[nodiscard]] OperationResultType binaryOperationType(LexemType opType,
                                                          const Type& rhs) const override;
    [[nodiscard]] OperationResultType prefixOperationType(LexemType opType) const override;

    [[nodiscard]] bool isBoolean() const override;
    [[nodiscard]] BooleanType& getBool() override;
    [[nodiscard]] const BooleanType& getBool() const override;
};

#endif  // VUG_BOOLEAN_TYPE_HPP
