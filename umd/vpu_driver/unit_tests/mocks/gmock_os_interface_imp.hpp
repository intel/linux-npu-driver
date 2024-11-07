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

class GMockOsFileImp : public OsFile {
  public:
    MOCK_METHOD(bool, write, (const void *, size_t), (override));
    MOCK_METHOD(void *, mmap, (), (override));
    MOCK_METHOD(size_t, size, (), (override));
};

class GMockOsInterfaceImp : public OsInterface {
  public:
    MOCK_METHOD(int, osiOpen, (const char *, int, mode_t), (override));
    MOCK_METHOD(int, osiClose, (int), (override));
    MOCK_METHOD(int, osiFcntl, (int, int), (override));
    MOCK_METHOD(int, osiIoctl, (int, unsigned long, void *), (override));
    MOCK_METHOD(size_t, osiGetSystemPageSize, (), (override));
    MOCK_METHOD(void *, osiMmap, (void *, size_t, int, int, int, off_t), (override));
    MOCK_METHOD(int, osiMunmap, (void *, size_t), (override));
    MOCK_METHOD(std::string, osiReadFile, (const std::filesystem::path &, size_t), (override));
    MOCK_METHOD(bool, osiCreateDirectories, (const std::filesystem::path &), (override));
    MOCK_METHOD(std::unique_ptr<OsFile>,
                osiOpenWithExclusiveLock,
                (const std::filesystem::path &, bool),
                (override));
    MOCK_METHOD(std::unique_ptr<OsFile>,
                osiOpenWithSharedLock,
                (const std::filesystem::path &, bool),
                (override));
    MOCK_METHOD(void,
                osiScanDir,
                (const std::filesystem::path &, std::function<void(const char *, struct stat &)>),
                (override));
    MOCK_METHOD(bool, osiFileRemove, (const std::filesystem::path &), (override));
};

} // namespace VPU
