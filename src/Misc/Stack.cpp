// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at
// https://mozilla.org/MPL/2.0/.

#include "Stack.hpp"

#ifdef _WIN32

#define _WIN32_WINNT 0x0602  // NOLINT(*-reserved-identifier)
#define NOMINMAX
#include <windows.h>

#include <processthreadsapi.h>
#include <cstdint>

void setStackBottom() {
    uintptr_t low{};
    uintptr_t high{};
    GetCurrentThreadStackLimits(&low, &high);
    stackBottom = low;
}
#endif

#ifndef _WIN32
void setStackBottom() {
}
#endif

// TODO! Add unix-like systems support
