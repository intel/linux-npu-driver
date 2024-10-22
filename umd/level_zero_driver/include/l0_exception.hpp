/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero/ze_api.h"
#include "vpu_driver/source/utilities/log.hpp"

#include <stdexcept>

namespace L0 {

#define L0_THROW_WHEN(condition, message, result)        \
    if (condition) {                                     \
        LOG_E("%s, condition: %s", message, #condition); \
        throw DriverError(result);                       \
    }

#define L0_HANDLE_EXCEPTION_AND_RETURN(function_call)   \
    try {                                               \
        return function_call;                           \
    } catch (const std::exception &e) {                 \
        LOG_E("Exception caught, msg: '%s'", e.what()); \
        return ZE_RESULT_ERROR_UNKNOWN;                 \
    }

class DriverError : public std::exception {
  public:
    DriverError(ze_result_t r)
        : r(r) {}

    ze_result_t result() const { return r; }
    const char *what() const noexcept override { return "Driver error occurred"; }

  private:
    ze_result_t r;
};

} // namespace L0
