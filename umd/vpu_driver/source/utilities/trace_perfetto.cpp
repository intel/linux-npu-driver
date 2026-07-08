/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include "perfetto.h"

#include "trace_perfetto.hpp"

#include "vpu_driver/source/utilities/log.hpp"

#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>

#if ENABLE_NPU_PERFETTO_BUILD

PERFETTO_TRACK_EVENT_STATIC_STORAGE();

static TracePerfetto trace_perfetto;

namespace {

static perfetto::protos::pbzero::CounterDescriptor::Unit toPerfettoUnit(VPU::CounterUnit unit) {
    switch (unit) {
    case VPU::CounterUnit::Bytes:
        return perfetto::protos::pbzero::CounterDescriptor::UNIT_SIZE_BYTES;

    case VPU::CounterUnit::None:
    default:
        return perfetto::protos::pbzero::CounterDescriptor::UNIT_UNSPECIFIED;
    }
}

} // namespace

#endif

#if ENABLE_NPU_PERFETTO_BUILD

TracePerfetto::TracePerfetto() {
    const char *env = getenv("ZE_INTEL_NPU_LOGPERFETTO");
    enable = env == nullptr || env[0] == '0' || env[0] == '\0' ? false : true;

    if (enable) {
        bool isSystemBackend = std::string(env) == "system";

        perfetto::TracingInitArgs args;
        if (!isSystemBackend) {
            args.backends |= perfetto::kInProcessBackend;
        } else {
            args.backends |= perfetto::kSystemBackend;
        }
        perfetto::Tracing::Initialize(args);
        perfetto::TrackEvent::Register();

        perfetto::protos::gen::TrackEventConfig track_event_cfg;
        track_event_cfg.add_disabled_categories("*");
        track_event_cfg.add_enabled_categories("API");
        track_event_cfg.add_enabled_categories("SYS");
        track_event_cfg.add_enabled_categories("NPU_ELF");
        track_event_cfg.add_enabled_categories("NPU_COMPILER");
        track_event_cfg.add_enabled_categories("NPU_MEMORY");

        if (!isSystemBackend) {
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
}

TracePerfetto::~TracePerfetto() {
    if (!record)
        return;

    record->StopBlocking();

    std::vector<char> trace_data(record->ReadTraceBlocking());

    std::ofstream out;
    out.open("intel-npu-umd.perfetto-trace", std::ios::out | std::ios::binary);
    out.write(&trace_data[0], static_cast<long>(trace_data.size()));
    out.close();
}

#endif

namespace VPU {

struct DriverCounter::TrackImpl {
#if ENABLE_NPU_PERFETTO_BUILD
    std::string trackName;
    perfetto::CounterTrack track;

    TrackImpl(const std::string &name, CounterUnit unit)
        : trackName(name)
        , track(perfetto::DynamicString(trackName), toPerfettoUnit(unit), perfetto::Track()) {}
#else
    TrackImpl(const std::string &, CounterUnit) {}
#endif
};

DriverCounter::DriverCounter(const std::string &counterName, CounterUnit unit)
    : name(counterName)
    , value(0)
    , track(std::make_unique<TrackImpl>(counterName, unit)) {}

DriverCounter::~DriverCounter() = default;

void DriverCounter::add(size_t x) {
#if ENABLE_NPU_PERFETTO_BUILD
    if (!trace_perfetto.isEnabled())
        return;

    const int64_t valueToAdd = static_cast<int64_t>(x);

    int64_t newVal = value.fetch_add(valueToAdd, std::memory_order_relaxed) + valueToAdd;

    publish(newVal);

#else
    (void)x;
#endif
}

void DriverCounter::sub(size_t x) {
#if ENABLE_NPU_PERFETTO_BUILD
    if (!trace_perfetto.isEnabled())
        return;

    const int64_t valueToSub = static_cast<int64_t>(x);

    int64_t newVal = value.fetch_sub(valueToSub, std::memory_order_relaxed) - valueToSub;

    if (newVal < 0) {
        LOG_E("DriverCounter underflow: counter=%s value=%ld delta=%zu", name.c_str(), newVal, x);
    }

    publish(newVal);
#else
    (void)x;
#endif
}

void DriverCounter::publish(int64_t v) {
#if ENABLE_NPU_PERFETTO_BUILD
    TRACE_COUNTER("NPU_MEMORY", track->track, v);
#else
    (void)v;
#endif
}

} // namespace VPU
