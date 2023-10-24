/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "test_app.h"
#include "version.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <fstream>
#include <thread>

#include "perf_counter.h"
#include "gtest/gtest.h"

static bool g_list_tests = false;
static pid_t g_parent_pid;

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

static void print_help(const char *test_app_name) {
    printf("\n");

    printf("VPU Specific Options:\n"
           "  [FILTER]\n"
           "      Test selection filter (shortcut for --gtest_filter=FILTER).\n"
           "  -D/--run_disabled\n"
           "      Run disabled tests (shortcut for --gtest_also_run_disabled_tests).\n"
           "  -S/--run_skipped\n"
           "      Run skipped tests.\n"
           "  -r/--repeat <COUNT>\n"
           "      Repeat tests COUNT times (shortcut for --gtest_repeat=COUNT).\n"
           "  -l/--list_tests\n"
           "      List all tests in TEST_GROUP.TEST_NAME format.\n"
           "  -v/--verbose\n"
           "      Print verbose debug traces.\n"
           "  -t <timeout_msec>\n"
           "       Override default timeout for performance tests\n"
           "  -p/--pause_after_test_ms <pause_ms>\n"
           "       Add delay in ms after test completion\n"
           "  -c/--config [CONFIGURATION_PATH]\n"
           "       Test configuration file in yaml format\n"
           "  --version\n"
           "       Display application version\n"

           "\n");

    printf("Example Usage:\n");
    printf("  %s -v Device.*\n", test_app_name);
    printf("  %s -l\n", test_app_name);
    printf("  %s *:-Device*:Memory*\n", test_app_name);
    printf("  %s --gtest_filter=*:-Device*:Memory*\n", test_app_name);
    printf("  %s --gtest_filter=*:-Device*:Memory*\n", test_app_name);
    printf("  %s --config=basic.yaml\n", test_app_name);

    printf("\n");
}

static struct option options[] = {{"help", no_argument, NULL, 'h'},
                                  {"list_tests", no_argument, NULL, 'l'},
                                  {"verbose", no_argument, NULL, 'v'},
                                  {"run_disabled", no_argument, NULL, 'D'},
                                  {"run_skipped", no_argument, NULL, 'S'},
                                  {"repeat", required_argument, NULL, 'r'},
                                  {"timeout_msec", required_argument, NULL, 't'},
                                  {"pause_after_test_ms", required_argument, NULL, 'p'},
                                  {"config", required_argument, NULL, 'c'},
                                  {"version", no_argument, NULL, 0x10},
                                  {NULL, 0, NULL, '0'}};

namespace test_app {

LogLevel log_level;
bool run_skipped_tests;
unsigned pause_after_test_ms;

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

    TRACE("%s with size=%ld @ %p\n", name, size, data);
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

void parse_args(int argc, char **argv) {
    int c, r;

    ::testing::InitGoogleTest(&argc, argv);

    while (1) {
        c = getopt_long(argc, argv, "hlvDSr:t:p:c:", options, NULL);
        if (c < 0)
            break;

        switch (c) {
        case 'h':
            print_help(argv[0]);
            exit(1);
        case 'v':
            test_app::log_level = DEBUG;
            break;
        case 'l':
            g_list_tests = true;
            break;
        case 'D':
            ::testing::GTEST_FLAG(also_run_disabled_tests) = true;
            break;
        case 'r':
            if (optarg)
                r = atoi(optarg);
            else
                r = 1;
            if (r > 0)
                ::testing::GTEST_FLAG(repeat) = r;
            break;
        case 'S':
            test_app::run_skipped_tests = true;
            break;
        case 't':
            if (optarg)
                PerfCounter::default_timeout_msec = atoi(optarg);
            if (PerfCounter::default_timeout_msec < 1) {
                fprintf(stderr, "Bad command line option.\n");
                exit(1);
            }
            break;
        case 'p':
            if (optarg)
                test_app::pause_after_test_ms = atoi(optarg);
            break;
        case 'c':
            break;
        case 0x10:
            printf("%s\n", vpu_drv_version_str);
            exit(1);
        default:
            fprintf(stderr, "Bad command line option.\n");
            exit(1);
        }
    }

    // use single non-option argument as a filter
    if (optind < argc) {
        ::testing::GTEST_FLAG(filter) = argv[optind];
        optind++;
    }

    // report additional non-option argument as error
    if (optind != argc) {
        fprintf(stderr, "Bad command line option: %s\n", argv[optind]);
        exit(1);
    }
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

bool is_vpu37xx(uint16_t pci_id) {
    switch (pci_id) {
    case PCI_ID_MTL_H:
    case PCI_ID_MTL_S:
        return true;
    default:
        return false;
    }
}

bool is_vpu(uint16_t pci_id) {
    return is_vpu37xx(pci_id);
}

} // namespace test_app
