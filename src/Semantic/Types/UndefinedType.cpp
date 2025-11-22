// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "UndefinedType.hpp"

OperationResultType UndefinedType::binaryOperationType(const LexemType opType,
                                                       const Type& rhs) const {
    return OperationResultType(true, this);
}
OperationResultType UndefinedType::prefixOperationType(const LexemType opType) const {
    return OperationResultType(true, this);
}

bool UndefinedType::isUndefined() const {
    return true;
}

