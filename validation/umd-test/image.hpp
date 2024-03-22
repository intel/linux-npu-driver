/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <cstddef>
#include <memory>
#include <string>

struct Image {
    Image(const std::string &path);
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
