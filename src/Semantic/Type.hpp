// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_TYPE_HPP
#define VUG_TYPE_HPP

#include <memory>
#include <utility>

#include "Lexing/Lexer.hpp"

class SymbolContext;
class Type;

enum class TypeKind {
    Undefined,
    Primitive,
};

enum class PrimitiveKind {
    Integer,
    Boolean
};

struct OperationResultType {
    bool isTypesCorrect;
    const Type* resultType;

    explicit OperationResultType(bool isTypesCorrect, const Type* resultType)
        : isTypesCorrect(isTypesCorrect), resultType(resultType) {}
};

class Type {
public:
    explicit Type(SymbolContext& context, std::string typeName, TypeKind kind)
        : _context(context),
          _typeName(std::move(typeName)),
          _kind(kind) {}

    virtual ~Type() = default;

    [[nodiscard]] virtual OperationResultType binaryOperationType(LexemType opType, const Type& rhs) const = 0;
    [[nodiscard]] virtual OperationResultType prefixOperationType(LexemType opType) const = 0;

    [[nodiscard]] TypeKind getKind() const {
        return _kind;
    }

    [[nodiscard]] const std::string& getTypeName() const {
        return _typeName;
    }

    [[nodiscard]] bool isInteger() const;

    inline bool operator==(const Type& rhs) const {
        return this == &rhs;
    }
    inline bool operator!=(const Type& rhs) const {
        return this != &rhs;
    }

protected:
    SymbolContext& _context;
    std::string _typeName;
    TypeKind _kind;
};

class UndefinedType : public Type {
    explicit UndefinedType(SymbolContext& context)
        : Type(context, "Undefined", TypeKind::Undefined) {}

public:
    [[nodiscard]] OperationResultType binaryOperationType(LexemType opType, const Type& rhs) const override;
    [[nodiscard]] OperationResultType prefixOperationType(LexemType opType) const override;
};

class PrimitiveType : public Type {
public:
    PrimitiveType(SymbolContext& context, std::string typeName, PrimitiveKind primitiveKind)
        : Type(context, std::move(typeName), TypeKind::Primitive),
          _primitiveKind(primitiveKind) {}

    [[nodiscard]] PrimitiveKind getPrimitiveKind() const {
        return _primitiveKind;
    }

protected:
    PrimitiveKind _primitiveKind;
};

class BooleanType : public PrimitiveType {
public:
    BooleanType(SymbolContext& context, std::string typeName)
        : PrimitiveType(context, std::move(typeName), PrimitiveKind::Boolean) {}

    [[nodiscard]] OperationResultType binaryOperationType(LexemType opType, const Type& rhs) const override;
    [[nodiscard]] OperationResultType prefixOperationType(LexemType opType) const override;
};

class IntegerType : public PrimitiveType {
public:
    IntegerType(SymbolContext& context, std::string typeName, uint32_t bits, bool isSigned)
        : PrimitiveType(context, std::move(typeName), PrimitiveKind::Integer),
          _bits(bits),
          _isSigned(isSigned) {}

    [[nodiscard]] OperationResultType binaryOperationType(LexemType opType, const Type& rhs) const override;
    [[nodiscard]] OperationResultType prefixOperationType(LexemType opType) const override;

    [[nodiscard]] uint32_t getBits() const {
        return _bits;
    }
    [[nodiscard]] bool isIsSigned() const {
        return _isSigned;
    }

protected:
    uint32_t _bits;
    bool _isSigned;
};


#endif//VUG_TYPE_HPP
