/*
 * Copyright (C) 2022-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero/ze_api.h"

namespace L0 {

class IContextObject {
  public:
    virtual ~IContextObject() = default;
};

} // namespace L0
