/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdio.h>
#include <string>
#include <gtest/gtest.h>
#include <getopt.h>

#define ARRAY_SIZE(s) (sizeof(s) / sizeof(s[0]))

#define PRINTF(str, ...) std::fprintf(stderr, str, ##__VA_ARGS__)
#define TRACE(...)             \
    if (test_app::log_level) { \
        PRINTF(__VA_ARGS__);   \
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

enum LogLevel { NONE = 0, DEBUG = 1 };

extern LogLevel log_level;

extern std::string app_path;
extern bool run_skipped_tests;
extern bool use_sleep_wait;
extern unsigned pause_after_test_ms;

int hex_dump(void *data, long size, const char *name);

void parse_args(int argc, char **argv);
void append_negative_filter(const char *negative_pattern);
int run();

bool is_vpu37xx(uint16_t pci_id);
bool is_vpu40xx(uint16_t pci_id);
bool is_vpu(uint16_t pci_id);

} // namespace test_app
