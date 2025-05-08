/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstddef>

#include <memory>
#include <string>

struct Image {
    // If layoutChw is false, the data is in HWC format. Otherwise, data is in CHW format
    Image(const std::string &path, bool layoutChw);
    ~Image();

    void *getPtr();
    size_t getHeight() const;
    size_t getWidth() const;
    size_t getChannels() const;
    size_t getElementSize() const;
    size_t getSizeInBytes() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
