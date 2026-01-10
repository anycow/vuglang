// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#ifndef VUG_STACK_HPP
#define VUG_STACK_HPP

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <source_location>
#include <stdexcept>

thread_local inline uintptr_t stackBottom = 0;
constexpr size_t stackEpsilon = static_cast<const size_t>(128 * 1024);

void setStackBottom();

inline bool checkStackCapacity() {
    const size_t freeStack = std::abs(reinterpret_cast<intptr_t>(__builtin_frame_address(0))
                                      - static_cast<intptr_t>(stackBottom));
    return freeStack >= stackEpsilon;
}

#ifdef _WIN32
inline void stackGuard(const std::source_location location = std::source_location::current()) {
    if (!checkStackCapacity()) {
        std::cerr
            << "Stack Overflow at "
            << location.function_name()
            << " in "
            << std::filesystem::path(location.file_name()).filename()
            << std::endl;
        throw std::overflow_error(("Stack overflow"));
    }
}
#endif
#ifndef _WIN32
inline void stackGuard(std::source_location location = std::source_location::current()) {
}
#endif


#endif  // VUG_STACK_HPP
