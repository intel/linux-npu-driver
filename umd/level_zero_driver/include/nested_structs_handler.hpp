/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <optional>

namespace L0 {

// clang-format off
// Structures defined in level-zero have `pNext` field which can point to a different structure.
// For example, we can have the following chain of objects:
//
// +------------------------+        +----------------------------+        +------------------------------------------+
// | ze_device_properties_t |    +-->| ze_device_ip_version_ext_t |    +-->| ze_mutable_command_list_exp_properties_t |
// +------------------------+    |   +----------------------------+    |   +------------------------------------------+
// | stype                  |    |   | stype                      |    |   | stype                                    |
// +------------------------+    |   +----------------------------+    |   +------------------------------------------+
// | pNext                  |----+   | pNext                      |----+   | pNext = nullptr                          |
// +------------------------+        +----------------------------+        +------------------------------------------+
// |  ...                   |        |  ...                       |        |  ...                                     |
// +------------------------+        +----------------------------+        +------------------------------------------+
//
// This function iterates over that chain of objects and applies user supplied function to each of the object.
// Parameters:
// - `pNext` - it's a pointer to an object in the chain
// - `handler` - function that is applied to an object in the chain
// - `args` - additional arguments that are passed to the `handler`
//
// `handler` must return std::optional<T*> and its first argument must be of type T*.
//
// handleNestedStructs returns false if:
// - there's more than `NESTED_STRUCTS_LIMIT` nested objects, or
// - `handler` returns std::optional that doesn't have a value
// clang-format on
//

template <typename T, typename F, typename... Args>
bool handleNestedStructs(T *pNext, const F &handler, Args &...args) {
    // keep the count of nested structs and limit the number of iterations to prevent cycles
    static constexpr int NESTED_STRUCTS_LIMIT = 20;
    int count = 0;

    while (pNext != nullptr) {
        if (count >= NESTED_STRUCTS_LIMIT) {
            return false;
        }

        std::optional<T *> result = handler(pNext, args...);

        if (!result.has_value())
            return false;

        pNext = *result;
        count++;
    }

    return true;
}

} // namespace L0
