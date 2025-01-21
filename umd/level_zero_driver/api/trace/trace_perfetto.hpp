/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#if ENABLE_NPU_PERFETTO_BUILD
#include <memory>
#include <perfetto.h>

PERFETTO_DEFINE_CATEGORIES(perfetto::Category("API").SetDescription("Intel NPU UMD"));

namespace L0 {

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

} // namespace L0

#else

#define PERFETTO_TRACK_EVENT_STATIC_STORAGE()
#define TRACE_EVENT_BEGIN(...) \
    do {                       \
    } while (0)
#define TRACE_EVENT_END(...) \
    do {                     \
    } while (0)

namespace L0 {

class TracePerfetto {
  public:
    TracePerfetto() = default;
    ~TracePerfetto() = default;

    TracePerfetto(TracePerfetto &&) = delete;
    TracePerfetto(const TracePerfetto &) = delete;
    TracePerfetto &&operator=(TracePerfetto &&) = delete;
    TracePerfetto &operator=(const TracePerfetto &) = delete;
};

} // namespace L0
#endif
