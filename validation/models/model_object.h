/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <array>

typedef struct internal_model {
    const char *name;
    const uint8_t *xml;
    const uint8_t *bin;
    const uint8_t *conf;
    const size_t xml_len;
    const size_t bin_len;
    const size_t conf_len;
} internal_model_t;

extern const std::array<internal_model_t, 2> embeddedModel;
