// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_TYPE_HPP
#define VUG_TYPE_HPP

#include <utility>

#include "Lexing/Lexer.hpp"

class SymbolContext;
class Type;
class PrimitiveType;
class IntegerType;
class BooleanType;

enum class TypeKind {
    Undefined,
    Primitive,
};

enum class PrimitiveKind {
    Integer,
    Boolean,
};

struct OperationResultType {
    bool isTypesCorrect;
    const Type* resultType;

    constexpr explicit OperationResultType(const bool isTypesCorrect, const Type* resultType)
        : isTypesCorrect(isTypesCorrect),
          resultType(resultType) {
    }
};

class Type {
   public:
    constexpr explicit Type(SymbolContext& context, std::string typeName, const TypeKind kind)
        : mContext(context),
          mTypeName(std::move(typeName)),
          mKind(kind) {
    }

    virtual ~Type() = default;

    [[nodiscard]] virtual OperationResultType binaryOperationType(LexemType opType,
                                                                  const Type& rhs) const
        = 0;
    [[nodiscard]] virtual OperationResultType prefixOperationType(LexemType opType) const = 0;


    [[nodiscard]] SymbolContext& getContext() const {
        return mContext;
    }
    [[nodiscard]] constexpr TypeKind getKind() const {
        return mKind;
    }
    [[nodiscard]] constexpr const std::string& getTypeName() const {
        return mTypeName;
    }

    [[nodiscard]] virtual bool isUndefined() const;
    [[nodiscard]] virtual bool isPrimitive() const;
    [[nodiscard]] virtual bool isInteger() const;
    [[nodiscard]] virtual bool isBoolean() const;

    [[nodiscard]] virtual PrimitiveType& getPrimitive();
    [[nodiscard]] virtual const PrimitiveType& getPrimitive() const;
    [[nodiscard]] virtual IntegerType& getInteger();
    [[nodiscard]] virtual const IntegerType& getInteger() const;
    [[nodiscard]] virtual BooleanType& getBool();
    [[nodiscard]] virtual const BooleanType& getBool() const;

    constexpr bool operator==(const Type& rhs) const {
        return this == &rhs;
    }
    constexpr bool operator!=(const Type& rhs) const {
        return this != &rhs;
    }

   private:
    SymbolContext& mContext;
    std::string mTypeName;
    TypeKind mKind;
};

#endif  // VUG_TYPE_HPP
