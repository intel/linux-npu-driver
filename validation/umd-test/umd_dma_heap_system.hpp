/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "umd_test.h"

#include <fcntl.h>
#include <filesystem>
#include <linux/dma-buf.h>
#include <linux/dma-heap.h>
#include <memory>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define CHECK_DMA_HEAP_SUPPORT(x)                                          \
    SKIP_CHROMEOS();                                                       \
    if (!x.checkDmaHeapSystemAvailability()) {                             \
        const char *msg = "Missed support or insufficient permissions for" \
                          " dma buffer allocation in the system.";         \
        if (test_vars::forceDmaHeap) {                                     \
            GTEST_FAIL() << msg;                                           \
        } else {                                                           \
            GTEST_SKIP() << msg;                                           \
        }                                                                  \
    }

namespace test_vars {
extern bool forceDmaHeap;
}

class DmaHeapBuffer {
  public:
    DmaHeapBuffer(int fd, void *ptr, size_t size)
        : fd(fd)
        , ptr(ptr)
        , size(size) {}
    ~DmaHeapBuffer() {
        if (ptr != MAP_FAILED)
            ::munmap(ptr, size);
        if (fd >= 0)
            ::close(fd);
    }

    DmaHeapBuffer(const DmaHeapBuffer &obj) = delete;
    DmaHeapBuffer(DmaHeapBuffer &&obj) = delete;
    DmaHeapBuffer &operator=(const DmaHeapBuffer &obj) = delete;
    DmaHeapBuffer &operator=(DmaHeapBuffer &&obj) = delete;

  public:
    int fd;
    void *ptr;
    size_t size;
};

class DmaHeapSystem {
  public:
    DmaHeapSystem() {
        fd = open("/dev/dma_heap/system", O_RDWR);
        if (fd < 0) {
            printf("Failed to open /dev/dma_heap/system, errno: %i, strerror: %s\n",
                   errno,
                   strerror(errno));
        }
    }
    ~DmaHeapSystem() {
        if (fd >= 0)
            close(fd);
    }

    DmaHeapSystem(const DmaHeapSystem &obj) = delete;
    DmaHeapSystem(DmaHeapSystem &&obj) = delete;
    DmaHeapSystem &operator=(const DmaHeapSystem &obj) = delete;
    DmaHeapSystem &operator=(DmaHeapSystem &&obj) = delete;

    bool checkDmaHeapSystemAvailability() { return fd >= 0; }

    std::unique_ptr<DmaHeapBuffer> allocDmaHeapBuffer(size_t size) {
        if (!size) {
            printf("size has to be greater than 0\n");
            return nullptr;
        }

        struct dma_heap_allocation_data heapAlloc = {
            .len = ALLIGN_TO_PAGE(size),
            .fd = 0,
            .fd_flags = O_RDWR | O_CLOEXEC,
            .heap_flags = 0,
        };

        int ret = ioctl(fd, DMA_HEAP_IOCTL_ALLOC, &heapAlloc);
        if (ret != 0) {
            printf("ioctl failed with DMA_HEAP_IOCTL_ALLOC, errno: %u, strerror: %s\n",
                   errno,
                   strerror(errno));
            return nullptr;
        }

        int bufferFd = heapAlloc.fd;
        void *ptr = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, bufferFd, 0);
        if (ptr == MAP_FAILED) {
            printf("mmap failed on dma_heap buffer, errno: %u, strerror: %s\n",
                   errno,
                   strerror(errno));
            return nullptr;
        }

        return std::make_unique<DmaHeapBuffer>(bufferFd, ptr, size);
    }

  private:
    int32_t fd = -1;
};
