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
        : _context(context),
          _typeName(std::move(typeName)),
          _kind(kind) {
    }

    virtual ~Type() = default;

    [[nodiscard]] virtual OperationResultType binaryOperationType(LexemType opType,
                                                                  const Type& rhs) const
        = 0;
    [[nodiscard]] virtual OperationResultType prefixOperationType(LexemType opType) const = 0;

    [[nodiscard]] constexpr TypeKind getKind() const {
        return _kind;
    }

    [[nodiscard]] constexpr const std::string& getTypeName() const {
        return _typeName;
    }

    [[nodiscard]] bool isInteger() const;

    constexpr bool operator==(const Type& rhs) const {
        return this == &rhs;
    }
    constexpr bool operator!=(const Type& rhs) const {
        return this != &rhs;
    }

   protected:
    SymbolContext& _context;
    std::string _typeName;
    TypeKind _kind;
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
          _primitiveKind(primitiveKind) {
    }

    [[nodiscard]] constexpr PrimitiveKind getPrimitiveKind() const {
        return _primitiveKind;
    }

   protected:
    PrimitiveKind _primitiveKind;
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
          _bits(bits),
          _isSigned(isSigned) {
    }

    [[nodiscard]] OperationResultType binaryOperationType(LexemType opType,
                                                          const Type& rhs) const override;
    [[nodiscard]] OperationResultType prefixOperationType(LexemType opType) const override;

    [[nodiscard]] constexpr uint32_t getBits() const {
        return _bits;
    }
    [[nodiscard]] constexpr bool isIsSigned() const {
        return _isSigned;
    }

   protected:
    uint32_t _bits;
    bool _isSigned;
};


#endif  // VUG_TYPE_HPP
