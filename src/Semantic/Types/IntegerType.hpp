// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_INTEGER_TYPE_HPP
#define VUG_INTEGER_TYPE_HPP

#include "PrimitiveType.hpp"

class SymbolContext;

class IntegerType : public PrimitiveType {
   public:
    constexpr IntegerType(SymbolContext& context,
                          std::string typeName,
                          const uint32_t bits,
                          const bool isSigned)
        : PrimitiveType(context, std::move(typeName), PrimitiveKind::Integer),
          mBits(bits),
          mIsSigned(isSigned) {
    }

    [[nodiscard]] OperationResultType binaryOperationType(LexemType opType,
                                                          const Type& rhs) const override;
    [[nodiscard]] OperationResultType prefixOperationType(LexemType opType) const override;

    [[nodiscard]] bool isInteger() const override;
    [[nodiscard]] IntegerType& getInteger() override;
    [[nodiscard]] const IntegerType& getInteger() const override;

    [[nodiscard]] constexpr uint32_t getBits() const {
        return mBits;
    }
    [[nodiscard]] constexpr bool isSigned() const {
        return mIsSigned;
    }

   private:
    uint32_t mBits;
    bool mIsSigned;
};

#endif  // VUG_INTEGER_TYPE_HPP

