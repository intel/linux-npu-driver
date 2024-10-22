/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <getopt.h>
#include <gtest/gtest.h>
#include <limits>
#include <stdio.h>
#include <string>
#include <unordered_map>

#define ARRAY_SIZE(s) (sizeof(s) / sizeof(s[0]))

#define PRINTF(str, ...) std::fprintf(stderr, str, ##__VA_ARGS__)
#define TRACE(...)                \
    if (test_app::verbose_logs) { \
        PRINTF(__VA_ARGS__);      \
    }

#define TRACE_IN() TRACE("-->%s\n", __FUNCTION__)
#define TRACE_OUT() TRACE("<--%s\n", __FUNCTION__)
#define TRACE_PTR(p) TRACE("\t" #p "=%p\n", (void *)(uintptr_t)p)
#define TRACE_P32(p) TRACE("\t" #p "=0x%X\n", (unsigned int)(p))
#define TRACE_P64(p) TRACE("\t" #p "=0x%llX\n", (unsigned long long)(p))
#define TRACE_INT(d) TRACE("\t" #d "=%lld\n", (long long)(d))
#define TRACE_INT_STR(d, s) TRACE("\t" #d "=%d (%s)\n", d, s[d])
#define TRACE_STR(s) TRACE("\t" #s "=%s\n", s)
#define TRACE_STRUCT(s) test_app::hex_dump(&s, sizeof(s), #s)
#define TRACE_BUF(b, size) test_app::hex_dump(b, size, #b)

#define PCI_ID_MTL 0x7D1D
#define PCI_ID_ARL 0xAD1D
#define PCI_ID_LNL 0x643E

namespace test_app {

extern bool max_timeout;
extern bool verbose_logs;
extern bool run_skipped_tests;
extern unsigned pause_after_test_ms;

int hex_dump(void *data, long size, const char *name);

struct Argument {
    const char *longOpt;
    int hasArg;
    std::function<void(const char *)> callback;
};

using ArgumentMap = std::unordered_map<int, test_app::Argument>;

void parse_args(ArgumentMap &extArgs, const char *extHelpMsg, int argc, char **argv);
void append_positive_filter(const std::string &pattern);
void append_negative_filter(const char *negative_pattern);
int run();

template <typename T>
void overwrite_timeout(T &timeout) {
    if (test_app::max_timeout && timeout != 0)
        timeout = std::numeric_limits<T>::max();
}

bool is_vpu37xx(uint16_t pci_id);
bool is_vpu40xx(uint16_t pci_id);
bool is_vpu(uint16_t pci_id);

bool has_root_access();

} // namespace test_app
