/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/os_interface/os_interface.hpp"

#include <gmock/gmock.h>

namespace VPU {

// A GMock OS interface implementation.
class GMockOsInterfaceImp : public OsInterface {
  public:
    MOCK_METHOD(int, osiOpen, (const char *, int, mode_t), (override));
    MOCK_METHOD(int, osiClose, (int), (override));
    MOCK_METHOD(int, osiFcntl, (int, int), (override));
    MOCK_METHOD(int, osiIoctl, (int, unsigned long, void *), (override));
    MOCK_METHOD(void *, osiAlloc, (size_t), (override));
    MOCK_METHOD(int, osiFree, (void *), (override));
    MOCK_METHOD(size_t, osiGetSystemPageSize, (), (override));
    MOCK_METHOD(void *,
                osiMmap,
                (void *addr, size_t size, int prot, int flags, int fd, off_t offset),
                (override));
    MOCK_METHOD(int, osiMunmap, (void *addr, size_t size), (override));
};

} // namespace VPU
