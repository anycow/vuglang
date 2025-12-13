// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_RESULT_HPP
#define VUG_RESULT_HPP

#include <iostream>
#include <variant>

template <typename T, typename E>
class [[nodiscard]] Result {
   public:
    [[nodiscard]] static constexpr Result ok(T value) {
        return Result(value);
    }
    [[nodiscard]] static constexpr Result error(E error) {
        return Result(error);
    }

    [[nodiscard]] constexpr bool isOk() const {
        return std::holds_alternative<T>(mData);
    }
    [[nodiscard]] constexpr bool isError() const {
        return std::holds_alternative<E>(mData);
    }

    [[nodiscard]] constexpr T& value() {
        return std::get<T>(mData);
    }
    [[nodiscard]] constexpr const T& value() const {
        return std::get<T>(mData);
    }

    [[nodiscard]] constexpr const E& error() const {
        return std::get<E>(mData);
    }

    T unwrap() {
        if (isOk()) {
            return std::move(value());
        } else {
            std::cerr << "unwrap() called on error" << error() << std::endl;
            std::abort();
        }
    }

   private:
    std::variant<T, E> mData;
    constexpr explicit Result(T value)
        : mData(std::move(value)) {
    }
    constexpr explicit Result(E error)
        : mData(std::move(error)) {
    }
};

template <typename E>
class [[nodiscard]] Result<void, E> {
   public:
    [[nodiscard]] static constexpr Result ok() {
        return Result();
    }
    [[nodiscard]] static constexpr Result error(E error) {
        return Result(error);
    }

    [[nodiscard]] constexpr bool isOk() const {
        return !mError;
    }
    [[nodiscard]] constexpr bool isError() const {
        return mError.has_value();
    }

    [[nodiscard]] constexpr const E& error() const {
        return mError.value();
    }

    void unwrap() const {
        if (isError()) {
            std::cerr << "unwrap() called on error" << error() << std::endl;
            std::abort();
        }
    }

   private:
    std::optional<E> mError;
    constexpr explicit Result()
        : mError(std::nullopt) {
    }
    constexpr explicit Result(E error)
        : mError(std::move(error)) {
    }
};
#endif  // VUG_RESULT_HPP
