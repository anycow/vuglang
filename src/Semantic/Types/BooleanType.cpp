// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "BooleanType.hpp"

#include "Type.hpp"

OperationResultType BooleanType::binaryOperationType(const LexemType opType,
                                                     const Type& rhs) const {
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
OperationResultType BooleanType::prefixOperationType(const LexemType opType) const {
    switch (opType) {
        case LexemType::Not:
            return OperationResultType(true, this);
        default:
            return OperationResultType(false, nullptr);
    }
}

bool BooleanType::isBoolean() const {
    return true;
}

BooleanType& BooleanType::getBool() {
    return *this;
}
const BooleanType& BooleanType::getBool() const {
    return *this;
}

