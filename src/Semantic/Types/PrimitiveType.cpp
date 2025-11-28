// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "PrimitiveType.hpp"

bool PrimitiveType::isPrimitive() const {
    return true;
}

PrimitiveType& PrimitiveType::getPrimitive() {
    return *this;
}
const PrimitiveType& PrimitiveType::getPrimitive() const {
    return *this;
}

