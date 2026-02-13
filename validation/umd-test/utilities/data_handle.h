/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <cstdint>

#include <limits>
#include <linux/kernel.h>
#include <random>
#include <string>
#include <vector>

namespace DataHandle {

void generateRandomData(void *buffer, size_t size);

int loadFile(const std::string &filePath, std::vector<char> &dataOut);

int loadImageFile(const std::string &filePath, std::vector<char> &dataOut);

int writeFile(const std::string &filePath, const void *dataIn, size_t inputSize);

float rawFp16ToFp32(const uint16_t &data);

float rawBfp16ToFp32(const uint16_t &data);

template <class T>
T getRandomNumber(T from = std::numeric_limits<T>::min(), T to = std::numeric_limits<T>::max()) {
    thread_local std::mt19937 rd(std::random_device{}());
    std::uniform_int_distribution<T> dist(from, to);
    return dist(rd);
}

} // namespace DataHandle

// struct that represents float16 type and provides implicit conversions from/to float.
struct float16 {
    uint16_t value;

    float16(float f) {
        // use 32-bit `tmp` variable (instead of 16-bit `value`) as an argument for movss
        // to make sure movss instruction doesn't write out of bounds
        uint32_t tmp = 0;
        asm("movss %1, %%xmm6\n"
            "vcvtps2ph $0, %%xmm6, %%xmm7\n"
            "movss %%xmm7, %0\n"
            : "=m"(tmp)
            : "m"(f)
            : "xmm6", "xmm7");
        value = static_cast<uint16_t>(tmp);
    }

    operator float() const {
        float ret = 0;
        uint32_t tmp = static_cast<uint32_t>(value);

        asm("movss %1, %%xmm6\n"
            "vcvtph2ps %%xmm6, %%xmm7\n"
            "movss %%xmm7, %0\n"
            : "=m"(ret)
            : "m"(tmp)
            : "xmm6", "xmm7");

        return ret;
    }
};
static_assert(sizeof(float16) == 2);
