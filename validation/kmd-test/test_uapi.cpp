/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "drm_helpers.h"
#include "gtest/gtest.h"
#include "kmd_test.h"
#include "drm/ivpu_accel.h"

class UapiMem : public ::testing::TestWithParam<std::tuple<uint64_t, uint32_t>> {
  protected:
    static void SetUpTestSuite() { ctx.open(); }
    static void TearDownTestSuite() { ctx.close(); }

    void SetUp() override {
        uint64_t value;
        int ret = ctx.get_param(DRM_IVPU_PARAM_CAPABILITIES, &value, DRM_IVPU_CAP_DMA_MEMORY_RANGE);
        if (ret < 0 || value != 1)
            skip_dma = true;
    }

    static KmdContext ctx;
    bool skip_dma = false;
};

KmdContext UapiMem::ctx;

TEST_P(UapiMem, BoCreate) {
    auto [size, flags] = GetParam();
    uint32_t handle;
    uint64_t vpu_addr;

    if ((flags & DRM_IVPU_BO_DMA_MEM) && skip_dma)
        SKIP_("DMA memory range not supported");

    ASSERT_EQ(ctx.bo_create(size, flags, &handle, &vpu_addr), 0);
    ASSERT_EQ(ctx.bo_close(handle), 0);
}

TEST_F(UapiMem, BoCreateHuge) {
    const uint64_t HUGE_SIZE = 100 * MB;
    uint32_t handle;
    uint64_t vpu_addr;

    ASSERT_EQ(ctx.bo_create(HUGE_SIZE, 0, &handle, &vpu_addr), 0);
    ASSERT_EQ(ctx.bo_close(handle), 0);

    ASSERT_EQ(ctx.bo_create(HUGE_SIZE, DRM_IVPU_BO_SHAVE_MEM, &handle, &vpu_addr), 0);
    ASSERT_EQ(ctx.bo_close(handle), 0);

    if (skip_dma)
        return;

    ASSERT_EQ(ctx.bo_create(HUGE_SIZE, DRM_IVPU_BO_DMA_MEM, &handle, &vpu_addr), 0);
    ASSERT_EQ(ctx.bo_close(handle), 0);
}

TEST_F(UapiMem, BoCreateMultiple) {
    const uint64_t BUF_SIZE = 4 * KB;
    uint32_t handle1, handle2;
    uint64_t vpu_addr1, vpu_addr2;

    ASSERT_EQ(ctx.bo_create(BUF_SIZE, 0, &handle1, &vpu_addr1), 0);
    ASSERT_EQ(ctx.bo_create(BUF_SIZE, 0, &handle2, &vpu_addr2), 0);
    ASSERT_NE(handle1, handle2);
    ASSERT_NE(vpu_addr1, vpu_addr2);

    ASSERT_EQ(ctx.bo_close(handle1), 0);
    ASSERT_EQ(ctx.bo_close(handle2), 0);
}

TEST_F(UapiMem, BoCreateNegative) {
    uint32_t handle;
    uint64_t vpu_addr;

    ASSERT_NE(ctx.bo_create(4 * KB, UINT32_MAX, &handle, &vpu_addr), 0);
    ASSERT_EQ(handle, 0u);
    ASSERT_EQ(vpu_addr, 0u);

    ASSERT_NE(ctx.bo_create(4 * KB, DRM_IVPU_BO_UNCACHED | DRM_IVPU_BO_WC, &handle, &vpu_addr), 0);
    ASSERT_EQ(handle, 0u);
    ASSERT_EQ(vpu_addr, 0u);

    ASSERT_NE(ctx.bo_create(UINT64_MAX, 0, &handle, &vpu_addr), 0);
    ASSERT_EQ(handle, 0u);
    ASSERT_EQ(vpu_addr, 0u);

    ASSERT_NE(ctx.bo_create(0, 0, &handle, &vpu_addr), 0);
    ASSERT_EQ(handle, 0u);
    ASSERT_EQ(vpu_addr, 0u);
}

TEST_F(UapiMem, BoInfoNegative) {
    drm_ivpu_bo_info info = {};

    info.handle = UINT32_MAX;
    ASSERT_NE(ctx.bo_info(&info), 0);
}

TEST_F(UapiMem, BoInfoClosedNegative) {
    drm_ivpu_bo_info info = {};
    uint32_t handle;
    uint64_t vpu_addr;

    ASSERT_EQ(ctx.bo_create(4 * KB, 0, &handle, &vpu_addr), 0);
    info.handle = handle;
    ASSERT_EQ(ctx.bo_close(handle), 0);
    ASSERT_NE(ctx.bo_info(&info), 0);
}

TEST_P(UapiMem, BoMmap) {
    auto [size, flags] = GetParam();
    const uint8_t PATTERN = 0xbe;
    uint32_t handle;
    uint64_t vpu_addr;

    /* Skip all unmappable buffers */
    if (!(flags & DRM_IVPU_BO_MAPPABLE))
        return;

    if ((flags & DRM_IVPU_BO_DMA_MEM) && skip_dma)
        SKIP_("DMA memory range not supported");

    ASSERT_EQ(ctx.bo_create(size, flags, &handle, &vpu_addr), 0);

    drm_ivpu_bo_info info = {};
    info.handle = handle;
    ASSERT_EQ(ctx.bo_info(&info), 0);

    void *ptr = ctx.bo_mmap(size, PROT_READ | PROT_WRITE, info.mmap_offset);
    ASSERT_TRUE(ptr);

    memset(ptr, PATTERN, size);
    EXPECT_BYTE_ARR_EQ((uint8_t *)ptr, size, PATTERN);

    memset(ptr, ~PATTERN, size);
    EXPECT_BYTE_ARR_EQ((uint8_t *)ptr, size, ~PATTERN);

    ASSERT_EQ(munmap(ptr, size), 0);
    ASSERT_EQ(ctx.bo_close(handle), 0);
}

TEST_F(UapiMem, BoMmapNegative) {
    const uint64_t BUF_SIZE = 4 * KB;
    uint32_t handle;
    uint64_t vpu_addr;
    void *ptr;

    ASSERT_EQ(ctx.bo_create(4 * KB, DRM_IVPU_BO_MAPPABLE, &handle, &vpu_addr), 0);

    drm_ivpu_bo_info info = {};
    info.handle = handle;
    EXPECT_EQ(ctx.bo_info(&info), 0);

    /* bad size */
    EXPECT_EQ(ptr = ctx.bo_mmap(0, PROT_READ | PROT_WRITE, info.mmap_offset), nullptr);
    EXPECT_EQ(munmap(ptr, 0), -1);
    EXPECT_EQ(ptr = ctx.bo_mmap(BUF_SIZE + 1, PROT_READ | PROT_WRITE, info.mmap_offset), nullptr);
    EXPECT_EQ(munmap(ptr, BUF_SIZE + 1), 0);

    /* bad offset */
    EXPECT_EQ(ptr = ctx.bo_mmap(BUF_SIZE, PROT_READ | PROT_WRITE, 0), nullptr);
    EXPECT_EQ(munmap(ptr, BUF_SIZE), 0);
    EXPECT_EQ(ptr = ctx.bo_mmap(BUF_SIZE, PROT_READ | PROT_WRITE, UINT64_MAX), nullptr);
    EXPECT_EQ(munmap(ptr, BUF_SIZE), 0);
    EXPECT_EQ(ptr = ctx.bo_mmap(BUF_SIZE, PROT_READ | PROT_WRITE, info.mmap_offset + BUF_SIZE),
              nullptr);
    EXPECT_EQ(munmap(ptr, BUF_SIZE), 0);
    EXPECT_EQ(ptr = ctx.bo_mmap(BUF_SIZE, PROT_READ | PROT_WRITE, info.mmap_offset - 2 * BUF_SIZE),
              nullptr);
    EXPECT_EQ(munmap(ptr, BUF_SIZE), 0);

    /* mmap after close */
    EXPECT_EQ(ctx.bo_close(handle), 0);
    EXPECT_EQ(ptr = ctx.bo_mmap(BUF_SIZE, PROT_READ | PROT_WRITE, info.mmap_offset), nullptr);
    EXPECT_EQ(munmap(ptr, BUF_SIZE), 0);
}

INSTANTIATE_TEST_SUITE_P(
    ,
    UapiMem,
    ::testing::Combine(
        ::testing::Values(4 * KB, 64 * KB),
        ::testing::Values(0,
                          DRM_IVPU_BO_CACHED | DRM_IVPU_BO_SHAVE_MEM,
                          DRM_IVPU_BO_CACHED | DRM_IVPU_BO_DMA_MEM,
                          DRM_IVPU_BO_CACHED | DRM_IVPU_BO_MAPPABLE,
                          DRM_IVPU_BO_CACHED | DRM_IVPU_BO_SHAVE_MEM | DRM_IVPU_BO_MAPPABLE,
                          DRM_IVPU_BO_CACHED | DRM_IVPU_BO_DMA_MEM | DRM_IVPU_BO_MAPPABLE,
                          DRM_IVPU_BO_WC | DRM_IVPU_BO_SHAVE_MEM,
                          DRM_IVPU_BO_WC | DRM_IVPU_BO_DMA_MEM,
                          DRM_IVPU_BO_WC | DRM_IVPU_BO_MAPPABLE,
                          DRM_IVPU_BO_WC | DRM_IVPU_BO_SHAVE_MEM | DRM_IVPU_BO_MAPPABLE,
                          DRM_IVPU_BO_WC | DRM_IVPU_BO_DMA_MEM | DRM_IVPU_BO_MAPPABLE)),
    [](const testing::TestParamInfo<std::tuple<uint64_t, uint32_t>> &cmd) {
        std::string str;
        uint64_t size = std::get<0>(cmd.param);
        uint32_t flags = std::get<1>(cmd.param);

        if (size < KB) {
            str.append(std::to_string(size)).append("B");
        } else if (size < MB) {
            str.append(std::to_string(size / 1024)).append("kB");
        } else {
            str.append(std::to_string(size / 1024 / 1024)).append("MB");
        }

        if (flags & DRM_IVPU_BO_UNCACHED)
            str.append("_Uncached");
        else if (flags & DRM_IVPU_BO_WC)
            str.append("_Writecombined");
        else
            str.append("_Cached");

        if (flags & DRM_IVPU_BO_SHAVE_MEM)
            str.append("_HighMem");
        else if (flags & DRM_IVPU_BO_DMA_MEM)
            str.append("_DmaMem");
        else
            str.append("_Lowmem");

        if (flags & DRM_IVPU_BO_MAPPABLE)
            str.append("_Mappable");
        else
            str.append("_Unmappable");

        return str;
    });

class UapiSubmit : public KmdTest {
  public:
    KmdContext other_ctx;

    void SetUp() override {
        KmdTest::SetUp();
        ts_buf = std::make_unique<MemoryBuffer>(context, map_len, VPU_BUF_USAGE_TIMESTAMP_HEAP);
        cmd_buf = std::make_unique<CmdBuffer>(context, map_len, VPU_BUF_USAGE_BATCHBUFFER);
        ASSERT_EQ(ts_buf->create(), 0);
        ASSERT_EQ(cmd_buf->create(), 0);
        cmd_buf->add_ts_cmd(*ts_buf, 0);
        cmd_buf->prepare_bb_hdr();
        cmd_buf->prepare_params(ENGINE_COMPUTE, DRM_IVPU_JOB_PRIORITY_DEFAULT, &params);
    }
    void TearDown() override {
        ts_buf.reset();
        cmd_buf.reset();
        KmdTest::TearDown();
    }
    const uint32_t map_len = 4096;
    std::unique_ptr<MemoryBuffer> ts_buf;
    std::unique_ptr<CmdBuffer> cmd_buf;
    drm_ivpu_submit params = {};
};

TEST_F(UapiSubmit, WrongEngine) {
    params.engine = 0xdead;

    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}

TEST_F(UapiSubmit, NoBufHandles) {
    params.buffer_count = 0;

    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}

TEST_F(UapiSubmit, ManyBufHandles) {
    for (uint32_t i = 0; i < 1000; i++)
        cmd_buf->add_handle(*ts_buf);

    ASSERT_EQ(cmd_buf->submit_retry(&params), 0);
    ASSERT_EQ(cmd_buf->wait(), 0);

    uint32_t timestamp1 = *ts_buf->ptr32();
    EXPECT_GT(timestamp1, 0UL);
}

TEST_F(UapiSubmit, BufHandleTsZero) {
    cmd_buf->referenced_handles[1] = 0;

    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}

TEST_F(UapiSubmit, BufHandleCmdZero) {
    cmd_buf->referenced_handles[0] = 0;

    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}

TEST_F(UapiSubmit, BufHandleInvalid) {
    cmd_buf->referenced_handles[0] = 0xdead;

    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}

TEST_F(UapiSubmit, BufHandleForDeletedBuf) {
    ts_buf.reset();
    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}

TEST_F(UapiSubmit, BufHandleFromOtherContext) {
    KmdContext context2;
    int fd2 = other_ctx.open();
    ASSERT_GT(fd2, -1) << "open() failed with error " << errno << " - " << strerror(errno);

    // create many bufs in other_ctx to arrive at handle numbers that don't exist in first context
    MemoryBuffer other_buf1(other_ctx, map_len, VPU_BUF_USAGE_TIMESTAMP_HEAP);
    MemoryBuffer other_buf2(other_ctx, map_len, VPU_BUF_USAGE_TIMESTAMP_HEAP);
    MemoryBuffer other_buf3(other_ctx, map_len, VPU_BUF_USAGE_TIMESTAMP_HEAP);

    ASSERT_EQ(other_buf1.create(), 0);
    ASSERT_EQ(other_buf2.create(), 0);
    ASSERT_EQ(other_buf3.create(), 0);

    cmd_buf->add_handle(other_buf3);

    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}

// Cmd buf's referenced_handles are stored as array of u32. This test checks
// what happens when the array starts at address that's not aligned to sizeof(u32)
TEST_F(UapiSubmit, BufPointerMisaligned) {
    uint8_t *misaligned_storage = NULL;
    int offset = 3;
    int ret;
    int alignment = 8;
    size_t sizeof_handles = sizeof(uint32_t) * cmd_buf->referenced_handles.size();

    ret = posix_memalign((void **)&misaligned_storage, alignment, sizeof_handles + offset);
    ASSERT_EQ(ret, 0);
    memcpy(misaligned_storage + offset, cmd_buf->referenced_handles.data(), sizeof_handles);
    params.buffers_ptr = (uint64_t)(&misaligned_storage[offset]);
    ASSERT_EQ(cmd_buf->submit_retry(&params), 0);
    free(misaligned_storage);
}

TEST_F(UapiSubmit, BufPointerNull) {
    params.buffers_ptr = (uint64_t)NULL;
    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}

TEST_F(UapiSubmit, CmdHdrOffsetMisaligned) {
    params.commands_offset += 3;
    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}

TEST_F(UapiSubmit, CmdHdrOffsetOutsideBuffer) {
    // put some valid mem owned by this context at address where cmd hdr offset will point
    MemoryBuffer other_buf(context, map_len, VPU_BUF_USAGE_BATCHBUFFER);
    ASSERT_EQ(other_buf.create(), 0);
    cmd_buf->add_handle(other_buf);

    cmd_buf->prepare_params(ENGINE_COMPUTE, DRM_IVPU_JOB_PRIORITY_DEFAULT, &params);
    params.commands_offset = map_len + 1024;
    ASSERT_NE(cmd_buf->submit_retry(&params), 0);
}
