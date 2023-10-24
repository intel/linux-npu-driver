/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <mutex>
#include <thread>

enum CopyType { SYSTEM_TO_SYSTEM, LOCAL_TO_LOCAL };

struct CommandCopyPerfParam {
    CopyType copyType;
    size_t memorySize;
    int numIteration;
};

class CommandCopyPerf
    : public UmdTest,
      public ::testing::WithParamInterface<std::tuple<uint32_t, CommandCopyPerfParam>> {
  public:
    ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                         .pNext = nullptr,
                                         .ordinal = 0,
                                         .index = 0,
                                         .flags = 0,
                                         .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                         .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};

    ze_command_list_desc_t cmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                          .pNext = nullptr,
                                          .commandQueueGroupOrdinal = 0,
                                          .flags = 0};

    void CopyPerfTest(void *src,
                      void *dst,
                      size_t allocSize,
                      std::chrono::nanoseconds &duration,
                      uint64_t &timestamp);
};

INSTANTIATE_TEST_SUITE_P(
    ,
    CommandCopyPerf,
    ::testing::Combine(::testing::Values(1, 4, 8),
                       ::testing::Values(CommandCopyPerfParam{SYSTEM_TO_SYSTEM, 4096, 1},
                                         CommandCopyPerfParam{LOCAL_TO_LOCAL, 4096, 1})),
    [](const testing::TestParamInfo<std::tuple<uint32_t, CommandCopyPerfParam>> &info) {
        auto cpParam = std::get<1>(info.param);
        std::string str = std::to_string(std::get<0>(info.param)) + "_Threads_";
        switch (cpParam.copyType) {
        case SYSTEM_TO_SYSTEM:
            str += std::string("SYSTEM_TO_SYSTEM_");
            break;
        case LOCAL_TO_LOCAL:
            str += std::string("LOCAL_TO_LOCAL_");
            break;
        default:
            str = std::string("UNKNOWN_");
        }
        str = str.append(std::to_string(cpParam.memorySize)) + "B_";
        str = str.append(std::to_string(cpParam.numIteration)) + "_iterations";
        return str;
    });

void CommandCopyPerf::CopyPerfTest(void *src,
                                   void *dst,
                                   size_t allocSize,
                                   std::chrono::nanoseconds &duration,
                                   uint64_t &timestamp) {
    ze_result_t ret;
    auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto queue = scopedQueue.get();

    auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto list = scopedList.get();

    const size_t size = sizeof(uint64_t);
    auto tsMem = AllocSharedMemory(size * 2);
    uint64_t *ts = static_cast<uint64_t *>(tsMem.get());

    EXPECT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(zeCommandListAppendMemoryCopy(list, dst, src, allocSize, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts + 1, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    auto begin = std::chrono::steady_clock::now();
    EXPECT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    auto end = std::chrono::steady_clock::now();
    duration = end - begin;

    EXPECT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);

    EXPECT_LT(*ts, *(ts + 1));

    timestamp = *(ts + 1) - *ts;
}

TEST_P(CommandCopyPerf, AppendMemoryCopy) {
    auto param = GetParam();
    uint32_t numThreads = std::get<0>(param);
    auto cpParam = std::get<1>(param);

    ASSERT_GT(numThreads, 0) << "numThreads have to be greater than 0.";

    std::mutex mtx;

    long long int durationThreaded = 0;
    uint64_t sumTimestampValuesThreaded = 0u;

    std::chrono::nanoseconds minTimeThreaded = std::chrono::nanoseconds::max();
    std::chrono::nanoseconds maxTimeThreaded = std::chrono::nanoseconds::zero();

    std::vector<std::shared_ptr<void>> srcMem, dstMem;
    std::vector<void *> src, dst;

    for (uint32_t i = 0; i < numThreads; i++) {
        switch (cpParam.copyType) {
        case SYSTEM_TO_SYSTEM: {
            srcMem.push_back(AllocHostMemory(cpParam.memorySize));
            dstMem.push_back(AllocHostMemory(cpParam.memorySize));
            break;
        }
        case LOCAL_TO_LOCAL: {
            srcMem.push_back(AllocSharedMemory(cpParam.memorySize));
            dstMem.push_back(AllocSharedMemory(cpParam.memorySize));
            break;
        }
        default:
            FAIL() << "Invalid copyType parameter";
        }

        src.push_back(srcMem.back().get());
        dst.push_back(dstMem.back().get());

        memset(src.back(), 0xAB, cpParam.memorySize);
    }

    std::vector<std::shared_ptr<std::thread>> tasks;

    for (uint32_t j = 0; j < numThreads; j++) {
        tasks.push_back(std::make_unique<std::thread>([this,
                                                       &cpParam,
                                                       &mtx,
                                                       &durationThreaded,
                                                       &minTimeThreaded,
                                                       &maxTimeThreaded,
                                                       &sumTimestampValuesThreaded,
                                                       src = src[j],
                                                       dst = dst[j]]() {
            std::chrono::nanoseconds duration;
            long long int sumDuration = 0;
            uint64_t timestamp = 0u;
            uint64_t sumTimestampValues = 0u;

            std::chrono::nanoseconds minTime = std::chrono::nanoseconds::max();
            std::chrono::nanoseconds maxTime = std::chrono::nanoseconds::zero();

            for (int k = 0; k < cpParam.numIteration; k++) {
                CopyPerfTest(src, dst, cpParam.memorySize, duration, timestamp);

                minTime = std::min(duration, minTime);
                maxTime = std::max(duration, maxTime);

                sumDuration += duration.count();
                sumTimestampValues += timestamp;
            }

            {
                const std::lock_guard<std::mutex> lock(mtx);

                minTimeThreaded = std::min(minTime, minTimeThreaded);
                maxTimeThreaded = std::max(maxTime, maxTimeThreaded);

                durationThreaded += sumDuration;
                sumTimestampValuesThreaded += sumTimestampValues;
            }
        }));
    }

    for (const auto &t : tasks) {
        t.get()->join();
    }

    PRINTF("\nAverage Copy Command duration: %lld [ns]\n",
           (durationThreaded / cpParam.numIteration / numThreads));

    PRINTF("\nMinimum Copy Command execution time: %lld [ns]\n",
           static_cast<long long>(minTimeThreaded.count()));
    PRINTF("\nMaximum Copy Command execution time: %lld [ns]\n",
           static_cast<long long>(maxTimeThreaded.count()));

    PRINTF("\nAverage VPU device's timestamp value for Copy Command: %lld [VPU clock units]\n\n",
           static_cast<long long>(sumTimestampValuesThreaded) / cpParam.numIteration / numThreads);
}

/*
 Flags specifying allocation controls. Must be 0 (default)
 or a valid combination of ze_host_mem_alloc_flag_t
*/
struct AllocationControls {
    ze_host_mem_alloc_flags_t srcHostFlag;
    ze_host_mem_alloc_flags_t dstHostFlag;
};

class CommandCopyFlag
    : public UmdTest,
      public ::testing::WithParamInterface<std::tuple<uint64_t, uint32_t, AllocationControls>> {
  public:
    ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                         .pNext = nullptr,
                                         .ordinal = 0,
                                         .index = 0,
                                         .flags = 0,
                                         .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                         .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};

    ze_command_list_desc_t cmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                          .pNext = nullptr,
                                          .commandQueueGroupOrdinal = 0,
                                          .flags = 0};
};

INSTANTIATE_TEST_SUITE_P(
    SystemToSystem,
    CommandCopyFlag,
    ::testing::Combine(::testing::Values(256, 4 * KB), // allocation size
                       ::testing::Values(1, 8),        // number of copy commands in command list
                       ::testing::Values(AllocationControls{ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED,
                                                            ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED},
                                         AllocationControls{
                                             ZE_HOST_MEM_ALLOC_FLAG_BIAS_WRITE_COMBINED,
                                             ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED})),
    [](const testing::TestParamInfo<std::tuple<uint64_t, uint32_t, AllocationControls>> &info) {
        std::string str = "DMA_Size_" + memSizeToStr(std::get<0>(info.param)) + "_Commands_";
        str = str.append(std::to_string(std::get<1>(info.param))) + "_";

        auto allocationControls = std::get<2>(info.param);

        if (allocationControls.srcHostFlag == ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED) {
            str += std::string("Cached_");
        } else {
            str += std::string("WriteCombined_");
        }

        if (allocationControls.dstHostFlag == ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED) {
            str += std::string("_Cached");
        } else {
            str += std::string("_WriteCombined");
        }
        return str;
    });

TEST_P(CommandCopyFlag, MeasureCommandCopyUsingTimestamp) {
    uint64_t timestamp = 0u;
    auto [allocSize, numOfCopyCommands, allocationControls] = GetParam();

    std::vector<std::shared_ptr<void>> srcMem, dstMem;
    std::vector<void *> src, dst;

    for (uint32_t i = 0; i < numOfCopyCommands; i++) {
        srcMem.push_back(AllocHostMemory(allocSize, allocationControls.srcHostFlag));
        dstMem.push_back(AllocHostMemory(allocSize, allocationControls.dstHostFlag));

        src.push_back(srcMem.back().get());
        dst.push_back(dstMem.back().get());

        memset(src.back(), 0xAB, allocSize);
    }

    ze_device_properties_t devProp = {};
    devProp.stype = ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES;

    ASSERT_EQ(zeDeviceGetProperties(zeDevice, &devProp), ZE_RESULT_SUCCESS);
    uint64_t timestampFreq = devProp.timerResolution;

    ze_result_t ret;
    auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto queue = scopedQueue.get();

    auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto list = scopedList.get();

    const size_t size = sizeof(uint64_t);
    auto tsMem = AllocSharedMemory(size * 2);
    uint64_t *ts = static_cast<uint64_t *>(tsMem.get());

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    for (uint32_t j = 0; j < numOfCopyCommands; j++) {
        ASSERT_EQ(
            zeCommandListAppendMemoryCopy(list, dst[j], src[j], allocSize, nullptr, 0, nullptr),
            ZE_RESULT_SUCCESS);
    }

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts + 1, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_LT(*ts, *(ts + 1));

    timestamp = *(ts + 1) - *ts;

    PRINTF("\nVPU device's timestamp value for %u Copy Command(s): %lld [VPU clock units]\n",
           numOfCopyCommands,
           static_cast<long long>(timestamp));

    PRINTF("\nVPU device's timestamp value for %u Copy Command(s) in nanoseconds: %lld [ns]\n\n",
           numOfCopyCommands,
           static_cast<long long>(timestamp * timestampFreq));
}

TEST_P(CommandCopyFlag, MeasureCommandCopyUsingHostTime) {
    std::chrono::steady_clock::time_point start_time, stop_time;
    auto [allocSize, numOfCopyCommands, allocationControls] = GetParam();

    std::vector<std::shared_ptr<void>> srcMem, dstMem;
    std::vector<void *> src, dst;

    for (uint32_t i = 0; i < numOfCopyCommands; i++) {
        srcMem.push_back(AllocHostMemory(allocSize, allocationControls.srcHostFlag));
        dstMem.push_back(AllocHostMemory(allocSize, allocationControls.dstHostFlag));

        src.push_back(srcMem.back().get());
        dst.push_back(dstMem.back().get());

        memset(src.back(), 0xAB, allocSize);
    }

    start_time = std::chrono::steady_clock::now();
    ze_result_t ret;
    auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto queue = scopedQueue.get();

    auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto list = scopedList.get();

    for (uint32_t j = 0; j < numOfCopyCommands; j++) {
        ASSERT_EQ(
            zeCommandListAppendMemoryCopy(list, dst[j], src[j], allocSize, nullptr, 0, nullptr),
            ZE_RESULT_SUCCESS);
    }

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    stop_time = std::chrono::steady_clock::now();

    std::chrono::duration<long long, std::nano> jobDuration =
        std::chrono::duration_cast<std::chrono::duration<long long, std::nano>>(stop_time -
                                                                                start_time);

    PRINTF("\n%u Copy Command(s) execution time: %lld [ns]\n\n",
           numOfCopyCommands,
           jobDuration.count());
}
