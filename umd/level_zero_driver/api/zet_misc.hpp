/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/loader/ze_loader.h"
#include "level_zero/ze_api.h"

#include <string>

#pragma once

namespace L0 {
void *getLoaderHandle();
std::string getLoaderVersion();

ze_result_t translateHandle(zel_handle_type_t type, void *handler, void **pHandler);

template <class T>
static inline ze_result_t translateHandle(zel_handle_type_t type, T handler, T *pHandler) {
    return translateHandle(type, handler, reinterpret_cast<void **>(pHandler));
}

template <class T>
static inline ze_result_t translateHandle(zel_handle_type_t type, T &handler) {
    return translateHandle(type, handler, &handler);
}
} // namespace L0
