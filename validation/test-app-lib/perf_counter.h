/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>
#include <time.h>

// Performance counter utility class, useful for performance measurements
class PerfCounter {
  public:
    PerfCounter(unsigned timeout_msec = 100);
    ~PerfCounter();

    void setTimeout(unsigned timeout_msec);
    void start();
    void stop();
    void reset();
    double duration();
    double getFps();
    double getMbps();
    double getAvgTime();
    unsigned getCount();

    void printResult();
    void printResult(double result, const char *name);

    inline void countFrame(unsigned step = 1) { counter += step; }
    inline void countData(unsigned long long size, unsigned step = 1) {
        data_size += size;
        countFrame(step);
    }

    inline long long getTime() {
#define SEC_TO_NSEC(t) (t * INT64_C(1e9))
        struct timespec tv;

        clock_gettime(CLOCK_MONOTONIC, &tv);
        return tv.tv_nsec + SEC_TO_NSEC(tv.tv_sec);
    }

    inline bool isTimedOut() { return getTime() >= tout; }

    static int default_timeout_msec;

  private:
    static const int NSEC_IN_SEC = 1000000000;
    static const int NSEC_IN_MILISEC = 1000000;

    long long msec_to_nsec(int msec) { return (long long)msec * NSEC_IN_MILISEC; }

    unsigned counter;
    unsigned long long data_size;
    long long t0, t1, tout;
    bool valid;
    int test_timeout_msec;
};
