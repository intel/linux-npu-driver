/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <sys/mman.h>
#include <stdlib.h>
#include <thread>

#include "kmd_test.h"

class Copy : public KmdTest, public ::testing::WithParamInterface<std::tuple<__u16, __u32>> {
  protected:
    void CopyPerfTest(int engine, int buf_size, int copy_size, int repeats, bool coherent = true);
    void CopyDuringCtxCreation(int engine);

    void CopySystem2Local2System(int size, bool L2L) {
        const unsigned char pattern = 0xCD;

        MemoryBuffer src_buf(*this, size, VPU_BUF_USAGE_INPUT_HIGH);
        ASSERT_EQ(src_buf.create(), 0);
        src_buf.fill(pattern);
        MemoryBuffer mid1_buf(*this, size, VPU_BUF_USAGE_INPUT_OUTPUT_LOW);
        ASSERT_EQ(mid1_buf.create(), 0);
        mid1_buf.clear();
        MemoryBuffer mid2_buf(*this, size, VPU_BUF_USAGE_INPUT_OUTPUT_LOW);
        ASSERT_EQ(mid2_buf.create(), 0);
        mid2_buf.clear();
        MemoryBuffer dst_buf(*this, size, VPU_BUF_USAGE_OUTPUT_HIGH);
        ASSERT_EQ(dst_buf.create(), 0);
        dst_buf.clear();

        MemoryBuffer descr_buf(*this, 4096, VPU_BUF_USAGE_DESCRIPTOR_HEAP);
        ASSERT_EQ(descr_buf.create(), 0);

        CmdBuffer cmd_buf(context, 4096, VPU_BUF_USAGE_BATCHBUFFER);
        ASSERT_EQ(cmd_buf.create(), 0);

        cmd_buf.add_copy_cmd(descr_buf, 0, src_buf, 0, mid1_buf, 0, size);
        ASSERT_EQ(cmd_buf.submit(ENGINE_COPY), 0);
        ASSERT_EQ(cmd_buf.wait(), 0);

        if (L2L) {
            cmd_buf.start(0);
            cmd_buf.add_copy_cmd(descr_buf, 0, mid1_buf, 0, mid2_buf, 0, size);
            ASSERT_EQ(cmd_buf.submit(ENGINE_COMPUTE), 0);
            ASSERT_EQ(cmd_buf.wait(), 0);
        }

        cmd_buf.start(0);
        cmd_buf.add_copy_cmd(descr_buf, 0, L2L ? mid2_buf : mid1_buf, 0, dst_buf, 0, size);
        ASSERT_EQ(cmd_buf.submit(ENGINE_COPY), 0);
        ASSERT_EQ(cmd_buf.wait(), 0);

        ASSERT_EQ(*(uint32_t *)(src_buf.ptr()), *(uint32_t *)(dst_buf.ptr()));
        ASSERT_EQ(memcmp(src_buf.ptr(), dst_buf.ptr(), size), 0);
    }
};

TEST_F(Copy, CopySystemLocalSystem) {
    CopySystem2Local2System(1024, false);
}

TEST_F(Copy, CopySystemLocalSystemLarge) {
    CopySystem2Local2System(50 * 4096, false);
}

TEST_F(Copy, CopySystemLocalLocalSystem) {
    CopySystem2Local2System(1024, true);
}

TEST_P(Copy, Pattern) {
    const unsigned char pattern = 0xCD;
    int engine;
    auto [copyCommand, size] = GetParam();

    // rescale kB to bytes
    const int timeout_ms = (size > 1 * MB) ? 3 * JOB_SYNC_TIMEOUT_MS : JOB_SYNC_TIMEOUT_MS;

    if (copyCommand == VPU_CMD_COPY_LOCAL_TO_LOCAL)
        /*Implemented by Compute Engine*/
        engine = ENGINE_COMPUTE;
    else
        /*Implemented by Copy Engine*/
        engine = ENGINE_COPY;

    MemoryBuffer src_buf(*this,
                         size,
                         engine == ENGINE_COMPUTE ? VPU_BUF_USAGE_INPUT_LOW
                                                  : VPU_BUF_USAGE_INPUT_HIGH);
    ASSERT_EQ(src_buf.create(), 0);
    src_buf.fill(pattern);

    MemoryBuffer dst_buf(*this,
                         size,
                         engine == ENGINE_COMPUTE ? VPU_BUF_USAGE_OUTPUT_LOW
                                                  : VPU_BUF_USAGE_OUTPUT_HIGH);
    ASSERT_EQ(dst_buf.create(), 0);
    dst_buf.clear();

    MemoryBuffer descr_buf(*this, 4096, VPU_BUF_USAGE_DESCRIPTOR_HEAP);
    ASSERT_EQ(descr_buf.create(), 0);

    CmdBuffer cmd_buf(context, 4096, VPU_BUF_USAGE_BATCHBUFFER);
    ASSERT_EQ(cmd_buf.create(), 0);
    cmd_buf.add_copy_cmd(descr_buf, 0, src_buf, 0, dst_buf, 0, size, copyCommand);

    PerfCounter counter;
    counter.start();
    do {
        ASSERT_EQ(cmd_buf.submit(engine), 0);
        ASSERT_EQ(cmd_buf.wait(timeout_ms), 0);

        counter.countData(size);
    } while (!counter.isTimedOut() && is_silicon());
    counter.stop();

    ASSERT_EQ(*(uint32_t *)(src_buf.ptr()), *(uint32_t *)(dst_buf.ptr()));
    ASSERT_EQ(memcmp(src_buf.ptr(), dst_buf.ptr(), size), 0);
}

TEST_F(Copy, Loop) {
    const size_t src_buf_len = 4 * MB;
    const size_t dst_buf_len = 6 * MB;
    const size_t copy_len = 64;
    unsigned char pattern = 0x1A;
    const int total_loop = 20;
    std::vector<std::unique_ptr<MemoryBuffer>> src_bufs;
    std::vector<std::unique_ptr<MemoryBuffer>> dst_bufs;

    MemoryBuffer descr_buf(*this, 4096, VPU_BUF_USAGE_DESCRIPTOR_HEAP);
    ASSERT_EQ(descr_buf.create(), 0);

    CmdBuffer cmd_buf(context, 4096, VPU_BUF_USAGE_BATCHBUFFER);
    ASSERT_EQ(cmd_buf.create(), 0);

    for (int i = 0; i < total_loop; i++) {
        int src_offset = 16 * i;
        int dst_offset = 32 * i;
        TRACE_INT(i);
        src_bufs.push_back(
            std::make_unique<MemoryBuffer>(*this, src_buf_len, VPU_BUF_USAGE_INPUT_HIGH));
        ASSERT_EQ(src_bufs[i]->create(), 0)
            << "Failed to pin src buffer at iter " << i << std::endl;
        TRACE_P64(src_bufs[i]->vpu_addr());

        dst_bufs.push_back(
            std::make_unique<MemoryBuffer>(*this, dst_buf_len, VPU_BUF_USAGE_OUTPUT_HIGH));
        ASSERT_EQ(dst_bufs[i]->create(), 0)
            << "Failed to pin dst buffer at iter " << i << std::endl;
        TRACE_P64(dst_bufs[i]->vpu_addr());

        /* Setup new patter and clean dest buffer in each loop*/
        src_bufs[i]->fill(pattern++, src_offset, copy_len);
        dst_bufs[i]->clear(dst_offset, copy_len);

        cmd_buf.start(0);
        cmd_buf.add_copy_cmd(descr_buf,
                             0,
                             *src_bufs[i],
                             src_offset,
                             *dst_bufs[i],
                             dst_offset,
                             copy_len);
        ASSERT_EQ(cmd_buf.submit(ENGINE_COPY), 0);
        ASSERT_EQ(cmd_buf.wait(), 0);

        ASSERT_EQ(*(src_bufs[i]->ptr(src_offset)), *(dst_bufs[i]->ptr(dst_offset)))
            << "Copy first byte fail at iter " << i << std::endl;

        ASSERT_EQ(memcmp(dst_bufs[i]->ptr(dst_offset), src_bufs[i]->ptr(src_offset), copy_len), 0)
            << "Copy fail at iter " << i << std::endl;
    }
}

std::vector<std::tuple<__u16, __u32>> memtest_cases = {{VPU_CMD_COPY_LOCAL_TO_LOCAL, 4 * KB},
                                                       {VPU_CMD_COPY_LOCAL_TO_LOCAL, 64 * KB},
                                                       {VPU_CMD_COPY_LOCAL_TO_LOCAL, 4 * MB},
                                                       {VPU_CMD_COPY_LOCAL_TO_LOCAL, 16 * MB - 1}};

INSTANTIATE_TEST_SUITE_P(,
                         Copy,
                         ::testing::ValuesIn(memtest_cases),
                         [](const testing::TestParamInfo<std::tuple<__u16, __u32>> &cmd) {
                             std::string str;
                             switch (std::get<0>(cmd.param)) {
                             case VPU_CMD_COPY_LOCAL_TO_LOCAL:
                                 str = std::string("LocalToLocal_");
                                 break;
                             default:
                                 str = std::string("Unknown_");
                             }
                             __u32 size = std::get<1>(cmd.param);
                             if (size < KB) {
                                 str.append(std::to_string(size)).append("B");
                             } else if (size < MB) {
                                 str.append(std::to_string(size / 1024)).append("kB");
                             } else {
                                 str.append(std::to_string(size / 1024 / 1024)).append("MB");
                             }
                             return str;
                         });

void Copy::CopyPerfTest(int engine, int buf_len, int copy_len, int repeats, bool coherent) {
    unsigned char pattern = 0x1A;
    size_t desc_size = context.copy_desc_size();
    int cmd = VPU_CMD_COPY_LOCAL_TO_LOCAL;
    int flags = 0;

    if (engine == ENGINE_COPY) {
        flags = DRM_IVPU_BO_SHAVE_MEM;
    }

    MemoryBuffer descr_buf(*this, 4096, VPU_BUF_USAGE_DESCRIPTOR_HEAP);
    ASSERT_EQ(descr_buf.create(), 0);

    CmdBuffer cmd_buf(context, 4096, VPU_BUF_USAGE_BATCHBUFFER);
    ASSERT_EQ(cmd_buf.create(), 0);

    MemoryBuffer src_buf(context, buf_len);
    src_buf.set_flags(flags | DRM_IVPU_BO_MAPPABLE);
    ASSERT_EQ(src_buf.create(), 0);

    MemoryBuffer mid_buf1(context, buf_len);
    if (!coherent)
        mid_buf1.set_flags(flags | DRM_IVPU_BO_WC);
    ASSERT_EQ(mid_buf1.create(), 0);
    TRACE_P64(mid_buf1.vpu_addr());

    MemoryBuffer mid_buf2(context, buf_len);
    if (!coherent)
        mid_buf2.set_flags(flags | DRM_IVPU_BO_WC);
    ASSERT_EQ(mid_buf2.create(), 0);
    TRACE_P64(mid_buf2.vpu_addr());

    MemoryBuffer dst_buf(context, buf_len);
    dst_buf.set_flags(flags | DRM_IVPU_BO_MAPPABLE);
    ASSERT_EQ(dst_buf.create(), 0);
    TRACE_P64(dst_buf.vpu_addr());

    cmd_buf.add_copy_cmd(descr_buf, 0, src_buf, 0, mid_buf1, 0, copy_len, cmd);
    cmd_buf.add_copy_cmd(descr_buf, 1 * desc_size, mid_buf1, 0, mid_buf2, 0, copy_len, cmd);
    cmd_buf.add_copy_cmd(descr_buf, 2 * desc_size, mid_buf2, 0, dst_buf, 0, copy_len, cmd);

    for (int i = 0; i < repeats; i++) {
        TRACE_INT(i);
        // Update pattern in each loop
        src_buf.fill(pattern++, 0, copy_len);

        ASSERT_EQ(cmd_buf.submit(engine), 0);
        ASSERT_EQ(cmd_buf.wait(), 0);

        ASSERT_EQ(*(src_buf.ptr(0)), *(dst_buf.ptr(0)))
            << "Copy first byte fail at iter " << i << std::endl;
        ASSERT_EQ(memcmp(dst_buf.ptr(), src_buf.ptr(), copy_len), 0)
            << "Copy fail at iter " << i << std::endl;
    }
}

TEST_F(Copy, Coherent) {
    CopyPerfTest(ENGINE_COPY, 4 * KB, 4 * KB, 1, true);
}

TEST_F(Copy, NonCoherent) {
    CopyPerfTest(ENGINE_COPY, 4 * KB, 4 * KB, 1, false);
}

TEST_F(Copy, CoherentPerf) {
    CopyPerfTest(ENGINE_COPY, 1 * MB, 1 * MB, 2, true);
}

TEST_F(Copy, NonCoherentPerf) {
    CopyPerfTest(ENGINE_COPY, 1 * MB, 1 * MB, 2, false);
}

void Copy::CopyDuringCtxCreation(int engine) {
    const unsigned char pattern = 0xCD;
    bool stop = false;
    std::size_t size = 1 * MB;

    std::thread tdr_thread([&stop]() {
        while (!stop) {
            KmdContext thread_ctx;
            ASSERT_GE(thread_ctx.open(), 0);
            CmdBuffer cmd_buf(thread_ctx, 4096);
            ASSERT_EQ(cmd_buf.create(), 0);

            std::this_thread::yield();
        }
    });

    MemoryBuffer src_buf(*this, size, VPU_BUF_USAGE_INPUT_LOW);
    ASSERT_EQ(src_buf.create(), 0);
    src_buf.fill(pattern);

    MemoryBuffer dst_buf(*this, size, VPU_BUF_USAGE_INPUT_LOW);
    ASSERT_EQ(dst_buf.create(), 0);
    dst_buf.clear();

    MemoryBuffer descr_buf(*this, size, VPU_BUF_USAGE_DESCRIPTOR_HEAP);
    ASSERT_EQ(descr_buf.create(), 0);

    CmdBuffer cmd_buf(context, 4096, VPU_BUF_USAGE_BATCHBUFFER);
    ASSERT_EQ(cmd_buf.create(), 0);

    cmd_buf.start(0);
    cmd_buf.add_copy_cmd(descr_buf, 0, src_buf, 0, dst_buf, 0, size);
    ASSERT_EQ(cmd_buf.submit(engine), 0);
    ASSERT_EQ(cmd_buf.wait(), 0);

    stop = true;
    tdr_thread.join();
}

TEST_F(Copy, CopyDuringCtxCreationComputeEngine) {
    CopyDuringCtxCreation(ENGINE_COMPUTE);
}

TEST_F(Copy, CopyDuringCtxCreationCopyEngine) {
    CopyDuringCtxCreation(ENGINE_COPY);
}
