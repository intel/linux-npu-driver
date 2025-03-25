/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include <algorithm>
#include <chrono>
#include <limits>
#include <optional>
#include <stdio.h>
#include <thread>

constexpr static long MS_PER_SEC = 1'000;
constexpr static long US_PER_SEC = 1'000'000;
constexpr static long NS_PER_SEC = 1'000'000'000;

class FrameCounter {
  public:
    void startTimer(size_t timeoutSec, size_t expTargetFps) {
        targetFps = expTargetFps;
        frameCount = 0;
        fps = 0;
        frameMinMs = std::numeric_limits<double>::max();
        frameAvgMs = 0;
        frameMaxMs = 0;

        frameTargetSpan = std::chrono::microseconds(US_PER_SEC / expTargetFps);
        startPoint = std::chrono::steady_clock::now();
        framePoint = startPoint;
        frameTargetPoint = framePoint;
        timeoutPoint = startPoint + std::chrono::seconds(timeoutSec);

        running = true;
    }

    bool isTimeout() {
        if (!running)
            return true;

        return std::chrono::steady_clock::now() > timeoutPoint;
    }

    void delayNextFrame() {
        if (!running)
            return;

        while (std::chrono::steady_clock::now() < frameTargetPoint)
            std::this_thread::yield();
        framePoint = std::chrono::steady_clock::now();
    }

    void recordFrame(std::optional<double> latencyMs = std::nullopt) {
        if (!running)
            return;

        auto now = std::chrono::steady_clock::now();
        double frameSpanMs;
        if (latencyMs.has_value()) {
            frameSpanMs = latencyMs.value();
        } else {
            frameSpanMs = std::chrono::duration<double, std::milli>(now - framePoint).count();
        }

        frameTargetPoint = framePoint + frameTargetSpan;
        framePoint = now;

        frameMinMs = std::min(frameMinMs, frameSpanMs);
        frameAvgMs += frameSpanMs;
        frameMaxMs = std::max(frameMaxMs, frameSpanMs);
        frameCount++;
    }

    void stopTimer() {
        if (!running)
            return;

        std::chrono::duration<double, std::milli> durationMs =
            std::chrono::steady_clock::now() - startPoint;

        totalTimeMs = durationMs.count();
        if (frameCount > 0)
            frameAvgMs /= frameCount;
        if (totalTimeMs > 0)
            fps = MS_PER_SEC * frameCount / totalTimeMs;

        size_t frameExpected =
            targetFps * std::chrono::duration_cast<std::chrono::seconds>(durationMs).count();
        frameDrops = frameExpected < frameCount ? 0 : frameExpected - frameCount;
        running = false;
    }

    double getBytesPerSec(size_t bytes) const {
        size_t dataSize = bytes * frameCount;
        double totalTimeSec = totalTimeMs / MS_PER_SEC;
        return dataSize / totalTimeSec;
    }

    void printResult(const char *prefix) const {
        if (running) {
            std::chrono::duration<double, std::milli> durationMs =
                std::chrono::steady_clock::now() - startPoint;
            printf("%s: Running, totalTimeMs: %f,  frameCount: %lu\n",
                   prefix,
                   durationMs.count(),
                   frameCount);
            return;
        }

        printf("%s: Time[ms]: %.3f, FPS: %.3f, Frame Count/Drop: %lu/%lu, Frame Min/Avg/Max[ms]: "
               "%.3f/%.3f/%.3f\n",
               prefix,
               totalTimeMs,
               fps,
               frameCount,
               frameDrops,
               frameMinMs,
               frameAvgMs,
               frameMaxMs);
    }

    double totalTimeMs;
    size_t frameCount;
    size_t frameDrops;
    double fps;
    double frameMinMs;
    double frameAvgMs;
    double frameMaxMs;

  private:
    std::chrono::steady_clock::time_point startPoint;
    std::chrono::steady_clock::time_point framePoint;
    std::chrono::steady_clock::time_point frameTargetPoint;
    std::chrono::steady_clock::time_point timeoutPoint;
    std::chrono::microseconds frameTargetSpan;
    size_t targetFps;
    bool running = false;
};
