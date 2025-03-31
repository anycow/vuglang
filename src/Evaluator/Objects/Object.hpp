// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef VUG_OBJECT_HPP
#define VUG_OBJECT_HPP

#include <memory>
#include <string>

#include "Lexing/Token.hpp"

class Object {
public:
    virtual ~Object() = default;

    [[nodiscard]] virtual std::unique_ptr<Object> binaryOperation(LexemType opType, const Object& rhs) const = 0;
    [[nodiscard]] virtual std::unique_ptr<Object> prefixOperation(LexemType opType) const = 0;

    template<typename T>
    [[nodiscard]] T to() const {
        return static_cast<T>(*this);
    }

    virtual std::unique_ptr<Object> clone() = 0;
    virtual std::string toString() = 0;
};


#endif//VUG_OBJECT_HPP
