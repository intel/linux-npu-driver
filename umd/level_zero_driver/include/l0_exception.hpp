/*
 * Copyright (C) 2022-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdexcept>

#include "level_zero/ze_api.h"

namespace L0 {

#define L0_THROW_WHEN(condition, message, result)        \
    if (condition) {                                     \
        LOG_E("%s, condition: %s", message, #condition); \
        throw DriverError(result);                       \
    }

class DriverError : std::exception {
  public:
    DriverError(ze_result_t r)
        : r(r) {}

    ze_result_t result() const { return r; }

  private:
    ze_result_t r;
};

} // namespace L0
