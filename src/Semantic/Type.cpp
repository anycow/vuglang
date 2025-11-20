// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Type.hpp"

#include "Semantic/SymbolContext.hpp"

bool Type::isInteger() const {
    return _kind == TypeKind::Primitive && (static_cast<const PrimitiveType*>(this)->getPrimitiveKind() == PrimitiveKind::Integer);
}

OperationResultType BooleanType::binaryOperationType(LexemType opType, const Type& rhs) const {
    switch (opType) {
        case LexemType::Equal:
        case LexemType::Unequal:
        case LexemType::Less:
        case LexemType::LessEqual:
        case LexemType::Greater:
        case LexemType::GreaterEqual:
        case LexemType::LogicOr:
        case LexemType::LogicAnd:
            if (*this == rhs) {
                return OperationResultType(true, this);
            } else {
                return OperationResultType(false, nullptr);
            }
        default:
            return OperationResultType(false, nullptr);
    }
}
OperationResultType BooleanType::prefixOperationType(LexemType opType) const {
    switch (opType) {
        case LexemType::Not:
            return OperationResultType(true, this);
        default:
            return OperationResultType(false, nullptr);
    }
}

OperationResultType IntegerType::binaryOperationType(LexemType opType, const Type& rhs) const {
    switch (opType) {
        case LexemType::Equal:
        case LexemType::Unequal:
        case LexemType::Less:
        case LexemType::LessEqual:
        case LexemType::Greater:
        case LexemType::GreaterEqual:
            if (*this == rhs) {
                return OperationResultType(true, _context.getBoolType()->getType());
            } else {
                return OperationResultType(false, nullptr);
            }
        case LexemType::Plus:
        case LexemType::Minus:
        case LexemType::Multiply:
        case LexemType::Divide:
        case LexemType::Remainder:
        case LexemType::BitOr:
        case LexemType::BitXor:
        case LexemType::BitAnd:
            if (*this == rhs) {
                return OperationResultType(true, this);
            } else {
                return OperationResultType(false, nullptr);
            }
        default:
            return OperationResultType(false, nullptr);
    }
}
OperationResultType IntegerType::prefixOperationType(LexemType opType) const {
    switch (opType) {
        case LexemType::Minus:
            return OperationResultType(true, this);
        default:
            return OperationResultType(false, nullptr);
    }
}

OperationResultType UndefinedType::binaryOperationType(LexemType opType, const Type& rhs) const {
    return OperationResultType(true, this);
}
OperationResultType UndefinedType::prefixOperationType(LexemType opType) const {
    return OperationResultType(true, this);
}
