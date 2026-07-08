/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <atomic>
#include <memory>
#include <string>

#if ENABLE_NPU_PERFETTO_BUILD
#include <perfetto.h>

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category("API").SetDescription("Level Zero APIs traces to Intel NPU UMD"),
    perfetto::Category("SYS").SetDescription("Linux system calls used by Intel NPU UMD"),
    perfetto::Category("NPU_ELF").SetDescription("elf::HostParsedInference calls traces"),
    perfetto::Category("NPU_COMPILER").SetDescription("NPU Compiler API calls traces"),
    perfetto::Category("NPU_MEMORY").SetDescription("NPU memory usage counters"));
#endif

#if ENABLE_NPU_PERFETTO_BUILD
class TracePerfetto {
  public:
    TracePerfetto();
    ~TracePerfetto();

    TracePerfetto(const TracePerfetto &) = delete;
    TracePerfetto &operator=(const TracePerfetto &) = delete;
    TracePerfetto(TracePerfetto &&) = delete;
    TracePerfetto &operator=(TracePerfetto &&) = delete;

    bool isEnabled() const { return enable; }

  private:
    std::unique_ptr<perfetto::TracingSession> record;
    bool enable = false;
};
#endif

#if !ENABLE_NPU_PERFETTO_BUILD
#define TRACE_COUNTER(...) \
    do {                   \
    } while (0)

#define TRACE_EVENT(...) \
    do {                 \
    } while (0)

#define TRACE_EVENT_BEGIN(...) \
    do {                       \
    } while (0)

#define TRACE_EVENT_END(...) \
    do {                     \
    } while (0)
#endif

namespace VPU {

enum class CounterUnit { None, Bytes };

class DriverCounter {
  public:
    explicit DriverCounter(const std::string &counterName, CounterUnit unit = CounterUnit::None);

    ~DriverCounter();

    DriverCounter(const DriverCounter &) = delete;
    DriverCounter &operator=(const DriverCounter &) = delete;
    DriverCounter(DriverCounter &&) = delete;
    DriverCounter &operator=(DriverCounter &&) = delete;

    void add(size_t x);
    void sub(size_t x);

  private:
    void publish(int64_t v);

    struct TrackImpl;

    std::string name;
    std::atomic<int64_t> value = 0;

    std::unique_ptr<TrackImpl> track;
};

inline DriverCounter npuMemoryAllocatedByteCounter{"npu_memory_allocated_bytes",
                                                   CounterUnit::Bytes};

inline DriverCounter npuMemoryBufferCounter{"npu_memory_buffers"};

inline DriverCounter scratchCacheByteCounter{"scratch_cache_bytes", CounterUnit::Bytes};

inline DriverCounter scratchCacheBufferCounter{"scratch_cache_buffers"};

inline DriverCounter preemptionCacheByteCounter{"preemption_cache_bytes", CounterUnit::Bytes};

inline DriverCounter preemptionCacheBufferCounter{"preemption_cache_buffers"};

} // namespace VPU
