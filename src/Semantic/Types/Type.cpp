// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "Semantic/Types/Type.hpp"

#include <stdexcept>

bool Type::isUndefined() const {
    return false;
}
bool Type::isPrimitive() const {
    return false;
}
bool Type::isInteger() const {
    return false;
}
bool Type::isBoolean() const {
    return false;
}

PrimitiveType& Type::getPrimitive() {
    throw std::runtime_error("Type is not a PrimitiveType");
}
const PrimitiveType& Type::getPrimitive() const {
    throw std::runtime_error("Type is not a PrimitiveType");
}
IntegerType& Type::getInteger() {
    throw std::runtime_error("Type is not an IntegerType");
}
const IntegerType& Type::getInteger() const {
    throw std::runtime_error("Type is not an IntegerType");
}
BooleanType& Type::getBool() {
    throw std::runtime_error("Type is not a BooleanType");
}
const BooleanType& Type::getBool() const {
    throw std::runtime_error("Type is not a BooleanType");
}
