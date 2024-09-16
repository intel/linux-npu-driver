/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "kmd_test.h"
#include <thread>

class Exec : public KmdTest {
  public:
    void JobTest(int engine, int jobs);
    void JobPerf(int engine);

    void JobThread(int engine, uint64_t id);
    void JobThreadedTest(int thread_count, int repeat_count);
    void JobSubmitWithoutWait(int engine);
};

// Validate sending multiple jobs to the same context & engine
// Do not sync after each send to test queueing of jobs in the VPU
// and handling of job queue full condition in the KMD
void Exec::JobTest(int engine, int jobs) {
    std::vector<std::unique_ptr<CmdBuffer>> cmd_bufs;
    static const int min_pending_jobs = 6;
    int sync_index = 0;
    int ret;

    static constexpr int FENCE_OFFSET = 64;
    MemoryBuffer fence_buf(context, jobs * FENCE_OFFSET, VPU_BUF_USAGE_FENCE_HEAP);
    fence_buf.create();
    fence_buf.fill(0xA5, 0, 8);

    // Create command buffer for each job
    for (int i = 0; i < jobs; i++) {
        cmd_bufs.push_back(std::make_unique<CmdBuffer>(context, 4096, VPU_BUF_USAGE_BATCHBUFFER));
        ASSERT_EQ(cmd_bufs[i]->create(), 0);
    }

    // Send all jobs
    for (int i = 0; i < jobs; i++) {
        cmd_bufs[i]->start(0);
        cmd_bufs[i]->add_fence_signal_cmd(fence_buf, FENCE_OFFSET * i, i);

        TRACE("Sending job %d\n", i);
        ret = cmd_bufs[i]->submit(engine);

        if (ret == EBUSY) {
            // If KMD returns error, wait for oldest job to complete and retry
            if ((i - sync_index) > min_pending_jobs) {
                sync_index++;
                TRACE("Waiting for job %d\n", sync_index);
                ASSERT_EQ(cmd_bufs[sync_index]->wait(), 0)
                    << "Timeout waiting for job " << sync_index << std::endl;

                i--; // Retry job i
                continue;
            }
        };
        ASSERT_EQ(ret, 0) << "Submit failed at job " << i << std::endl;
    }

    // Sync and validate all buffers
    for (int i = 0; i < jobs; i++) {
        TRACE("Waiting for job %d\n", (i));
        ASSERT_EQ(cmd_bufs[i]->wait(), 0) << "Timeout waiting for job " << i << std::endl;

        ASSERT_EQ(*fence_buf.ptr64(FENCE_OFFSET * i), (uint64_t)i)
            << "Invalid fence at job " << i << std::endl;
    }
}

TEST_F(Exec, Jobs33_ComputeEngine) {
    JobTest(ENGINE_COMPUTE, 33);
}

TEST_F(Exec, Jobs133_Slow_ComputeEngine) {
    JobTest(ENGINE_COMPUTE, 133);
}

void Exec::JobThread(int engine, uint64_t id) {
    CmdBuffer cmd_buf(context, 4096);
    ASSERT_EQ(cmd_buf.create(), 0);

    cmd_buf.add_fence_signal_cmd(cmd_buf, 1024, id);

    ASSERT_EQ(cmd_buf.submit(engine), 0);
    ASSERT_EQ(cmd_buf.wait(), 0);

    EXPECT_EQ(*cmd_buf.ptr64(1024), id);
}

void Exec::JobThreadedTest(int thread_count, int repeat_count) {
    std::vector<std::unique_ptr<std::thread>> threads;

    for (int j = 0; j < repeat_count; j++) {
        for (int i = 0; i < thread_count; i++) {
            int id = 16 * i + j;
            int engine = i % 2;

            threads.push_back(std::make_unique<std::thread>(&Exec::JobThread, this, engine, id));
        }

        for (int i = 0; i < thread_count; i++) {
            threads[i]->join();
        }
        threads.clear();
    }
}

TEST_F(Exec, Threaded_t8_r3) {
    JobThreadedTest(8, 3);
}

void Exec::JobSubmitWithoutWait(int engine) {
    CmdBuffer cmd_buf(context, 4096);
    ASSERT_EQ(cmd_buf.create(), 0);

    cmd_buf.add_fence_wait_cmd(cmd_buf, 1024);
    ASSERT_EQ(cmd_buf.submit(engine), 0);

    cmd_buf.start(2048);
    cmd_buf.add_fence_signal_cmd(cmd_buf, 2048 + 1024);
    ASSERT_EQ(cmd_buf.submit(engine), EBUSY);

    CmdBuffer signal_buf(context, 4096);
    ASSERT_EQ(signal_buf.create(), 0);
    signal_buf.add_fence_signal_cmd(cmd_buf, 1024);

    if (engine == ENGINE_COPY)
        ASSERT_EQ(signal_buf.submit(ENGINE_COMPUTE), 0);
    else
        ASSERT_EQ(signal_buf.submit(ENGINE_COPY), 0);

    ASSERT_EQ(signal_buf.wait(), 0);
    ASSERT_EQ(cmd_buf.wait(), 0);
    ASSERT_EQ(*cmd_buf.ptr64(1024), FENCE_SIGNAL_VAL);
}

TEST_F(Exec, Job_SubmitWithoutWait_ComputeEngine) {
    JobSubmitWithoutWait(ENGINE_COMPUTE);
}

void Exec::JobPerf(int engine) {
    CmdBuffer cmd_buf(context, 4096);
    ASSERT_EQ(cmd_buf.create(), 0);

    cmd_buf.add_fence_signal_cmd(cmd_buf, 1024, 1234ull);
    ASSERT_EQ(cmd_buf.submit(engine), 0);
    ASSERT_EQ(cmd_buf.wait(), 0);

    PerfCounter counter;
    counter.setTimeout(is_silicon() ? 100 : 10);
    counter.start();
    do {
        ASSERT_EQ(cmd_buf.submit(engine), 0);
        ASSERT_EQ(cmd_buf.wait(), 0);
        counter.countFrame(1);
    } while (!counter.isTimedOut());
    counter.stop();
}

TEST_F(Exec, Perf_ComputeEngine) {
    JobPerf(ENGINE_COMPUTE);
}
