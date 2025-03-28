/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <functional>
#include <thread>

namespace test_app {

class thread {
  public:
    template <typename... Args>
    explicit thread(Args &&...args)
        : t(std::forward<Args>(args)...)
        , stop(false) {}

    ~thread() {
        if (joinable()) {
            request_stop();
            join();
        }
    }

    thread(const thread &) = delete;
    thread(thread &&other) noexcept
        : t(std::move(other.t))
        , stop(other.stop) {}

    thread &operator=(thread const &) = delete;

    thread &operator=(thread &&other) noexcept {
        if (joinable()) {
            request_stop();
            join();
        }

        t = std::move(other.t);
        stop = std::move(other.stop);

        return *this;
    }

    void swap(thread &other) noexcept {
        std::swap(stop, other.stop);
        std::swap(t, other.t);
    }

    bool joinable() const noexcept { return t.joinable(); }
    void join() { t.join(); }
    void detach() { t.detach(); }
    void request_stop() noexcept { stop = true; }
    bool stop_requested() const noexcept { return stop; }
    auto get_id() const noexcept { return t.get_id(); }

  private:
    std::thread t;
    volatile bool stop;
};

} // namespace test_app
