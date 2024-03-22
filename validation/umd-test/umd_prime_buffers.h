/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include <fcntl.h>
#include <linux/kernel.h>
#include <linux/dma-buf.h>
#include <linux/dma-heap.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define ALLIGN_TO_PAGE(x) __ALIGN_KERNEL((x), (UmdTest::PAGE_SIZE))

class PrimeBufferHelper {
  public:
    PrimeBufferHelper() {
        if (!std::filesystem::exists("/dev/dma_heap/system"))
            return;

        fd = open("/dev/dma_heap/system", O_RDWR);
    }
    PrimeBufferHelper(const PrimeBufferHelper &obj) = delete;
    PrimeBufferHelper(PrimeBufferHelper &&obj) = delete;
    PrimeBufferHelper &operator=(const PrimeBufferHelper &obj) = delete;
    PrimeBufferHelper &operator=(PrimeBufferHelper &&obj) = delete;

    ~PrimeBufferHelper() {
        for (const auto &[bufferFd, value] : buffers) {
            if (value.second != nullptr && value.second != MAP_FAILED)
                munmap(value.second, value.first);
            close(bufferFd);
        }
        if (fd >= 0)
            close(fd);
    }

    bool hasDMABufferSupport() {
        if (fd < 0)
            return false;
        return true;
    }

    bool createDMABuffer(size_t size, int32_t &bufferFd) {
        if (!hasDMABufferSupport() || !size)
            return false;

        struct dma_heap_allocation_data heapAlloc = {
            .len = ALLIGN_TO_PAGE(size),
            .fd = 0,
            .fd_flags = O_RDWR | O_CLOEXEC,
            .heap_flags = 0,
        };

        int ret = ioctl(fd, DMA_HEAP_IOCTL_ALLOC, &heapAlloc);
        if (ret != 0)
            return false;

        bufferFd = heapAlloc.fd;
        buffers.insert({heapAlloc.fd, {size, nullptr}});
        return true;
    }

    void *mmapDmaBuffer(int32_t &bufferFd) {
        if (auto it = buffers.find(bufferFd); it != buffers.end()) {
            if (it->second.second == nullptr)
                it->second.second =
                    mmap(NULL, it->second.first, PROT_WRITE | PROT_READ, MAP_SHARED, bufferFd, 0);
            return it->second.second;
        }
        return MAP_FAILED;
    }

    /* Descriptor for create exportable memory */
    ze_external_memory_export_desc_t externalExportDesc = {
        .stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC,
        .pNext = nullptr,
        .flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF};

    /* Descriptor to retrieve Fd through get properties */
    ze_external_memory_export_fd_t externalExportFdDesc = {
        .stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_FD,
        .pNext = nullptr,
        .flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF,
        .fd = -1};

    /* Descriptor to import allocation from fd */
    ze_external_memory_import_fd_t externalImportFromFdDesc = {
        .stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD,
        .pNext = nullptr,
        .flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF,
        .fd = -1};

  protected:
    using BufferFd = int32_t;
    using BufferSize = size_t;
    using BufferPtr = void *;
    std::map<BufferFd, std::pair<BufferSize, BufferPtr>> buffers;
    int32_t fd = -1;
};
