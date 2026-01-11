// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "UndefinedType.hpp"

#include "Lexing/Token.hpp"
#include "Semantic/Types/Type.hpp"

OperationResultType UndefinedType::binaryOperationType([[maybe_unused]] const LexemType opType,
                                                       [[maybe_unused]] const Type& rhs) const {
    return OperationResultType{true, this};
}
OperationResultType UndefinedType::prefixOperationType(
    [[maybe_unused]] const LexemType opType) const {
    return OperationResultType{true, this};
}

bool UndefinedType::isUndefined() const {
    return true;
}
