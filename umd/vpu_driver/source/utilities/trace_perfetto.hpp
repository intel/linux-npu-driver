/*
 * Copyright (C) 2024-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#if ENABLE_NPU_PERFETTO_BUILD
#include <memory>
#include <perfetto.h>

PERFETTO_DEFINE_CATEGORIES(
    perfetto::Category("API").SetDescription("Level Zero APIs traces to Intel NPU UMD"),
    perfetto::Category("SYS").SetDescription("Linux system calls used by Intel NPU UMD"),
    perfetto::Category("NPU_ELF").SetDescription("elf::HostParsedInference calls traces"),
    perfetto::Category("NPU_COMPILER").SetDescription("NPU Compiler API calls traces"));

class TracePerfetto {
  public:
    TracePerfetto();
    ~TracePerfetto();

    TracePerfetto(TracePerfetto &&) = delete;
    TracePerfetto(const TracePerfetto &) = delete;
    TracePerfetto &&operator=(TracePerfetto &&) = delete;
    TracePerfetto &operator=(const TracePerfetto &) = delete;

  private:
    std::unique_ptr<perfetto::TracingSession> record;
    bool enable = false;
};

#else

#define TRACE_EVENT(...) \
    do {                 \
    } while (0)
#define TRACE_EVENT_BEGIN(...) \
    do {                       \
    } while (0)
#define TRACE_EVENT_END(...) \
    do {                     \
    } while (0)

class TracePerfetto {
  public:
    TracePerfetto() = default;
    ~TracePerfetto() = default;

    TracePerfetto(TracePerfetto &&) = delete;
    TracePerfetto(const TracePerfetto &) = delete;
    TracePerfetto &&operator=(TracePerfetto &&) = delete;
    TracePerfetto &operator=(const TracePerfetto &) = delete;
};

#endif
