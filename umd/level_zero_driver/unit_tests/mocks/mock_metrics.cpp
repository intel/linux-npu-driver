/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/unit_tests/mocks/mock_metrics.hpp"

namespace L0 {
namespace ult {

void MockMetricQuery::injectDummyData() {
    size_t dataCount = metricGroup.getAllocationSize() / sizeof(uint64_t);

    uint64_t *metricData64 = reinterpret_cast<uint64_t *>(dataAddress);
    for (uint64_t i = 0; i < dataCount; i++) {
        metricData64[i] = 10 * (i + 1);
    }
}

} // namespace ult
} // namespace L0
