/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <gtest/gtest.h>
#include "perf_counter.h"

int PerfCounter::default_timeout_msec = -1;

PerfCounter::PerfCounter(unsigned timeout_msec)
    : counter(0)
    , data_size(0)
    , t0(0)
    , t1(0)
    , tout(0)
    , valid(false) {
    setTimeout(timeout_msec);
}

PerfCounter::~PerfCounter() {
    if (valid) {
        printResult();
    };
}

void PerfCounter::setTimeout(unsigned timeout_msec) {
    if (default_timeout_msec == -1)
        test_timeout_msec = timeout_msec;
    else
        test_timeout_msec = default_timeout_msec;
}

void PerfCounter::start() {
    counter = 0;
    data_size = 0;

    t0 = getTime();
    tout = t0 + msec_to_nsec(test_timeout_msec);
}

void PerfCounter::stop() {
    t1 = getTime();
    valid = counter || data_size;
}

void PerfCounter::reset() {
    counter = 0;
    data_size = 0;
    t0 = 0;
    valid = false;
}

double PerfCounter::duration() {
    return (t1 - t0) / (double)NSEC_IN_SEC;
}

double PerfCounter::getFps() {
    if (!counter) {
        return 0;
    }
    return ((double)counter) / duration();
}

double PerfCounter::getMbps() {
    return (((double)data_size) / duration()) / ((double)1024 * 1024);
}

double PerfCounter::getAvgTime() {
    if (counter == 0) {
        return 0.0;
    }
    return duration() / counter;
}

unsigned PerfCounter::getCount() {
    return counter;
}

void PerfCounter::printResult() {
    if (!data_size)
        return printResult(getFps(), "FPS");

    if (counter)
        return printResult(getMbps(), "MBps");

    printResult((double)data_size, "B");
}

void PerfCounter::printResult(double result, const char *name) {
    // Gets information about the currently running test.
    // Do NOT delete the returned object - it's managed by the UnitTest class.
    const ::testing::TestInfo *const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (!test_info) {
        return;
    }
    printf("[ BENCHMARK] %s.%s %.3f %s (%d loops)\n",
           test_info->test_case_name(),
           test_info->name(),
           result,
           name,
           counter);

    const size_t MAX_LEN = 256;
    char resultStr[MAX_LEN];
    snprintf(resultStr, MAX_LEN, "%lf", result);
    ::testing::Test::RecordProperty("score", resultStr);
    ::testing::Test::RecordProperty("score_desc", name);

    // Produce csv output
    if (0) {
        FILE *f = testing::internal::posix::FOpen("gtest_result.csv", "a+");
        if (f == NULL) {
            f = testing::internal::posix::FOpen("/tmp/gtest_result.csv", "a+");
        }
        if (f != NULL) {
            fprintf(f,
                    "%s.%s,%.3f,%s\n",
                    test_info->test_case_name(),
                    test_info->name(),
                    result,
                    name);
            fclose(f);
        }
    }
}
