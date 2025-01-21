/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include "perfetto.h"

#include "trace_perfetto.hpp"

#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>

namespace L0 {

TracePerfetto::TracePerfetto() {
    const char *env = getenv("ZE_INTEL_NPU_LOGPERFETTO");
    enable = env == nullptr || env[0] == '0' || env[0] == '\0' ? false : true;

    if (enable) {
        perfetto::TracingInitArgs args;
        args.backends |= perfetto::kInProcessBackend;
        perfetto::Tracing::Initialize(args);
        perfetto::TrackEvent::Register();

        perfetto::protos::gen::TrackEventConfig track_event_cfg;
        track_event_cfg.add_disabled_categories("*");
        track_event_cfg.add_enabled_categories("API");

        perfetto::TraceConfig cfg;
        cfg.add_buffers()->set_size_kb(64 * 1024);
        auto *ds_cfg = cfg.add_data_sources()->mutable_config();
        ds_cfg->set_name("track_event");
        ds_cfg->set_track_event_config_raw(track_event_cfg.SerializeAsString());

        record = perfetto::Tracing::NewTrace();
        record->Setup(cfg);
        record->StartBlocking();
    }
}

TracePerfetto::~TracePerfetto() {
    if (!record)
        return;

    record->StopBlocking();
    std::vector<char> trace_data(record->ReadTraceBlocking());

    std::ofstream out;
    out.open("intel-npu-umd.perfetto-trace", std::ios::out | std::ios::binary);
    out.write(&trace_data[0], (long)trace_data.size());
    out.close();
}

} // namespace L0
