// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_PRIMITIVE_TYPE_HPP
#define VUG_PRIMITIVE_TYPE_HPP

#include "Type.hpp"

class SymbolContext;

class PrimitiveType : public Type {
   public:
    constexpr PrimitiveType(SymbolContext& context,
                            std::string typeName,
                            const PrimitiveKind primitiveKind)
        : Type(context, std::move(typeName), TypeKind::Primitive),
          mPrimitiveKind(primitiveKind) {
    }

    [[nodiscard]] constexpr PrimitiveKind getPrimitiveKind() const {
        return mPrimitiveKind;
    }

    [[nodiscard]] bool isPrimitive() const override;
    [[nodiscard]] PrimitiveType& getPrimitive() override;
    [[nodiscard]] const PrimitiveType& getPrimitive() const override;

   private:
    PrimitiveKind mPrimitiveKind;
};

#endif  // VUG_PRIMITIVE_TYPE_HPP

