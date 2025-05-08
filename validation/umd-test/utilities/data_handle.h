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

int writeFile(const std::string &filePath, void *dataIn, size_t inputSize);

float rawFp16ToFp32(const uint16_t &data);

float rawBfp16ToFp32(const uint16_t &data);

template <class T>
T getRandomNumber(T from = std::numeric_limits<T>::min(), T to = std::numeric_limits<T>::max()) {
    thread_local std::mt19937 rd(std::random_device{}());
    std::uniform_int_distribution<T> dist(from, to);
    return dist(rd);
}

} // namespace DataHandle
