/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <linux/kernel.h>
#include <cstdint>
#include <string>
#include <vector>

namespace DataHandle {

void generateRandomData(std::vector<char> &data, size_t size);

int loadFile(const std::string &filePath, std::vector<char> &dataOut);

int loadImageFile(const std::string &filePath, std::vector<char> &dataOut);

int writeFile(const std::string &filePath, void *dataIn, size_t inputSize);

float rawFp16ToFp32(const uint16_t &data);

float rawBfp16ToFp32(const uint16_t &data);
} // namespace DataHandle
