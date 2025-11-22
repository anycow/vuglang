// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "IntegerType.hpp"

#include "Semantic/SymbolContext.hpp"
#include "Type.hpp"

OperationResultType IntegerType::binaryOperationType(const LexemType opType,
                                                     const Type& rhs) const {
    switch (opType) {
        case LexemType::Equal:
        case LexemType::Unequal:
        case LexemType::Less:
        case LexemType::LessEqual:
        case LexemType::Greater:
        case LexemType::GreaterEqual:
            if (*this == rhs) {
                return OperationResultType(true, getContext().getBoolType()->getType());
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
OperationResultType IntegerType::prefixOperationType(const LexemType opType) const {
    switch (opType) {
        case LexemType::Minus:
            return OperationResultType(true, this);
        default:
            return OperationResultType(false, nullptr);
    }
}

bool IntegerType::isInteger() const {
    return true;
}

IntegerType& IntegerType::getInteger() {
    return *this;
}
const IntegerType& IntegerType::getInteger() const {
    return *this;
}

