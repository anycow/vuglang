// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_TYPE_HPP
#define VUG_TYPE_HPP

#include <utility>

#include "Lexing/Lexer.hpp"

class SymbolContext;
class Type;

enum class TypeKind {
    Undefined,
    Primitive,
};

enum class PrimitiveKind { Integer, Boolean };

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

    [[nodiscard]] bool isInteger() const;

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

class UndefinedType : public Type {
   public:
    constexpr explicit UndefinedType(SymbolContext& context)
        : Type(context, "Undefined", TypeKind::Undefined) {
    }

    [[nodiscard]] OperationResultType binaryOperationType(LexemType opType,
                                                          const Type& rhs) const override;
    [[nodiscard]] OperationResultType prefixOperationType(LexemType opType) const override;
};

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

   private:
    PrimitiveKind mPrimitiveKind;
};

class BooleanType : public PrimitiveType {
   public:
    constexpr BooleanType(SymbolContext& context, std::string typeName)
        : PrimitiveType(context, std::move(typeName), PrimitiveKind::Boolean) {
    }

    [[nodiscard]] OperationResultType binaryOperationType(LexemType opType,
                                                          const Type& rhs) const override;
    [[nodiscard]] OperationResultType prefixOperationType(LexemType opType) const override;
};

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

    [[nodiscard]] constexpr uint32_t getBits() const {
        return mBits;
    }
    [[nodiscard]] constexpr bool isIsSigned() const {
        return mIsSigned;
    }

   private:
    uint32_t mBits;
    bool mIsSigned;
};


#endif  // VUG_TYPE_HPP
