/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "test_app.h"

#include "drm_helpers.h"
#include "gtest/gtest.h"
#include "perf_counter.h"
#include "version.h"

#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unordered_map>

namespace test_app {

bool g_list_tests = false;
pid_t g_parent_pid;
bool max_timeout = false;
bool verbose_logs = false;
bool run_skipped_tests;
unsigned pause_after_test_ms;

static int list_tests_with_full_names() {
    /* RUN_ALL_TESTS is required to apply filter. Redirect stdout to /dev/null to avoid unwanted
     * output */
    int fd = dup(1);
    int nullfd = open("/dev/null", O_WRONLY);

    if (nullfd >= 0) {
        dup2(nullfd, 1);
        close(nullfd);
    }

    int ret = RUN_ALL_TESTS();

    fflush(stdout);
    if (fd >= 0) {
        dup2(fd, 1);
        close(fd);
    }

    if (ret) {
        printf("ERROR: Failed to list the test with result %i\n", ret);
        return ret;
    }

    int count = ::testing::UnitTest::GetInstance()->total_test_suite_count();
    for (int i = 0; i < count; i++) {
        const ::testing::TestSuite *suite = ::testing::UnitTest::GetInstance()->GetTestSuite(i);
        int testCount = suite->total_test_count();
        for (int j = 0; j < testCount; j++) {
            const ::testing::TestInfo *testInfo = suite->GetTestInfo(j);
            if (testInfo == nullptr || !testInfo->should_run())
                continue;
            printf("%s.%s\n", testInfo->test_suite_name(), testInfo->name());
        }
    }
    return 0;
}

static void print_help(const char *extraMessage, const char *appName) {
    printf("\n");

    printf("Options:\n"
           "  [FILTER]\n"
           "      Test selection filter (shortcut for --gtest_filter=FILTER).\n"
           "  -D/--run_disabled\n"
           "      Run disabled tests (shortcut for --gtest_also_run_disabled_tests).\n"
           "  -S/--run_skipped\n"
           "      Run skipped tests.\n"
           "  -r/--repeat [COUNT]\n"
           "      Repeat tests COUNT times (shortcut for --gtest_repeat=COUNT).\n"
           "  -l/--list_tests\n"
           "      List all tests in TEST_GROUP.TEST_NAME format.\n"
           "  -v/--verbose\n"
           "      Print verbose debug traces.\n"
           "  -t/--timeout_msec [MSEC]\n"
           "       Override default timeout for performance tests\n"
           "  -p/--pause_after_test_ms [PAUSE_MS]\n"
           "       Add delay in ms after test completion\n"
           "  -V/--version\n"
           "       Display application version\n"
           "%s\n",
           extraMessage);

    printf("Environment Variables:\n"
           "  TESTS_DUMP_FW_LOG_ON_FAIL\n"
           "      Dump firmware log on failure.\n"
           "\n");

    printf("Example Usage:\n");
    printf("  %s -v Device.*\n", appName);
    printf("  %s -l\n", appName);
    printf("  %s *:-Device*:Memory.*\n", appName);
    printf("  %s --gtest_filter=*:-Device*:Memory.*\n", appName);

    printf("\n");
    exit(1);
}

static void setRunDisabledTests(const char *) {
    ::testing::GTEST_FLAG(also_run_disabled_tests) = true;
}

static void setDefaultPerfTimeout(const char *arg) {
    if (arg)
        PerfCounter::default_timeout_msec = atoi(arg);
    if (PerfCounter::default_timeout_msec < 1) {
        fprintf(stderr, "Bad command line option.\n");
        exit(1);
    }
}

static void setRepeatFlag(const char *arg) {
    ::testing::GTEST_FLAG(repeat) = atoi(arg);
}

static void setPauseAfterTestMs(const char *arg) {
    test_app::pause_after_test_ms = atoi(arg);
}

static void printVersion(const char *) {
    printf("%s\n", vpu_drv_version_str);
    exit(1);
}

static void convertToGetoptOptions(std::unordered_map<int, Argument> args,
                                   std::string &shortOpts,
                                   std::vector<option> &longOpts) {
    longOpts = {};
    shortOpts = "-";

    for (const auto &[shortOpt, content] : args) {
        if (std::isalpha(shortOpt)) {
            shortOpts += shortOpt;
            if (content.hasArg == required_argument)
                shortOpts += ":";
        }
        longOpts.push_back({content.longOpt, content.hasArg, nullptr, shortOpt});
    }
}

void parse_args(std::unordered_map<int, Argument> &extArgs,
                const char *extHelpMsg,
                int argc,
                char **argv) {
    ArgumentMap appArgs = {
        {'h', {"help", no_argument, [extHelpMsg, argv](auto) { print_help(extHelpMsg, argv[0]); }}},
        {'v', {"verbose", no_argument, [](auto) { test_app::verbose_logs = true; }}},
        {'D', {"run_disabled", no_argument, &setRunDisabledTests}},
        {'S', {"run_skipped", no_argument, [](auto) { test_app::run_skipped_tests = true; }}},
        {'r', {"repeat", required_argument, &setRepeatFlag}},
        {'l', {"list_tests", no_argument, [](auto) { g_list_tests = true; }}},
        {'t', {"timeout_msec", required_argument, &setDefaultPerfTimeout}},
        {'p', {"pause_after_test_ms", required_argument, &setPauseAfterTestMs}},
        {'V', {"version", no_argument, &printVersion}},
    };

    appArgs.merge(extArgs);

    std::vector<option> longOpts = {};
    std::string shortOpts = "-";
    convertToGetoptOptions(appArgs, shortOpts, longOpts);

    // InitGoogleTest is done before getopt to exclude --gtest* options from argv
    ::testing::InitGoogleTest(&argc, argv);

    while (1) {
        int opt = getopt_long(argc, argv, shortOpts.c_str(), longOpts.data(), NULL);
        if (opt == -1)
            break;

        if (opt == '\1') {
            ::testing::GTEST_FLAG(filter) = optarg;
            continue;
        }

        if (appArgs.count(opt)) {
            appArgs[opt].callback(optarg);
        } else {
            fprintf(stderr, "Bad command line option\n");
            exit(1);
        }
    }
}

void append_positive_filter(const std::string &pattern) {
    auto &filter = ::testing::GTEST_FLAG(filter);
    if (filter == "*")
        filter = pattern;
    else
        filter = pattern + ":" + filter;
}

void append_negative_filter(const char *negative_pattern) {
    std::string &filter = ::testing::GTEST_FLAG(filter);

    if (filter.find('-') == std::string::npos)
        filter += '-';
    filter += ':';
    filter += negative_pattern;
}

/*
 * Throw exception on ASSERT_*, so the test is interrupted immediately, even
 * if assertion failed in subroutine or contructor.
 * See https://github.com/google/googletest/blob/main/docs/advanced.md#propagating-fatal-failures
 */
class ThrowListener : public testing::EmptyTestEventListener {
    void OnTestPartResult(const testing::TestPartResult &result) override {
        if (result.type() == testing::TestPartResult::kFatalFailure) {
            if (g_parent_pid == getpid())
                throw testing::AssertionException(result);
        }
    }
};

int run() {
    /* Save parent pid not to throw exceptions from child processes */
    g_parent_pid = getpid();

    if (g_list_tests) {
        ::testing::GTEST_FLAG(list_tests) = true;
        return list_tests_with_full_names();
    }

    testing::UnitTest::GetInstance()->listeners().Append(new ThrowListener);

    return RUN_ALL_TESTS();
}

int hex_dump(void *data, long size, const char *name) {
    if (data == nullptr) {
        TRACE("Invalid data pointer! (%p)\n", data);
        return -1;
    }

    if (size <= 0 || size > INT16_MAX + 1) {
        TRACE("Invalid size! (%ld, max=%d)\n", size, INT16_MAX + 1);
        return -1;
    }

    auto *inputData = static_cast<uint8_t *>(data);

    TRACE("\n%s with size=%ld @ %p\n", name, size, data);
    TRACE("      x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF\n");
    TRACE("      -----------------------------------------------");
    for (long i = 0; i < size; i++) {
        if (i % 0x10 == 0) {
            TRACE("\n%04lX:", i);
        }

        const char *divider = (i % 0x10 == 0x08) ? "-" : " ";

        TRACE("%s%02X", divider, inputData[i]);
    }

    TRACE("\n");
    return 0;
}

bool is_vpu37xx(uint16_t pci_id) {
    switch (pci_id) {
    case PCI_ID_MTL:
    case PCI_ID_ARL:
        return true;
    default:
        return false;
    }
}

bool is_vpu40xx(uint16_t pci_id) {
    switch (pci_id) {
    case PCI_ID_LNL:
        return true;
    default:
        return false;
    }
}

bool is_vpu(uint16_t pci_id) {
    return is_vpu37xx(pci_id) || is_vpu40xx(pci_id);
}

bool has_root_access() {
    return geteuid() == 0;
}

} // namespace test_app
