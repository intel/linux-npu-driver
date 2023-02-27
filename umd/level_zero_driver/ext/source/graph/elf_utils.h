/*
 * Copyright (C) 2020-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdexcept>
#include <stdio.h>

#define check_cond_and_exit(cond, ...)                                     \
    {                                                                      \
        if ((cond) == false) {                                             \
            fprintf(stderr, __VA_ARGS__);                                  \
            throw std::invalid_argument("Invalid argument in elf parser"); \
        }                                                                  \
    }
