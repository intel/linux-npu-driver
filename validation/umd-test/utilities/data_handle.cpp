/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "data_handle.h"

#include <filesystem>
#include <fstream>
#include <random>
#include <string.h>

namespace DataHandle {

void generateRandomData(std::vector<char> &data, size_t size) {
    std::random_device rd;
    std::uniform_int_distribution<int8_t> dist;

    data.reserve(size);

    for (size_t i = 0; i < size; i++) {
        data.push_back(dist(rd));
    }
}

int loadFile(const std::string &filePath, std::vector<char> &dataOut) {
    std::ifstream fileInputStream(filePath, std::ios::binary | std::ios::ate);
    if (fileInputStream.is_open()) {
        std::streamsize size = fileInputStream.tellg();
        fileInputStream.seekg(0, std::ios::beg);

        dataOut.resize(size);
        fileInputStream.read((char *)dataOut.data(), size);
        fileInputStream.close();

        return 0;
    }
    return -1;
}

struct __attribute__((packed)) BmpFormat {
    uint16_t header;
    uint32_t size;
    uint32_t reserve;
    uint32_t offset;
};

int loadImageFile(const std::string &filePath, std::vector<char> &dataOut) {
    if (std::filesystem::exists(filePath) == false)
        return -1;

    std::vector<char> file;
    auto ret = loadFile(filePath, file);
    if (ret != 0)
        return ret;

    auto bmpHeader = reinterpret_cast<BmpFormat *>(file.data());
    dataOut.resize(bmpHeader->size - bmpHeader->offset);
    memcpy(dataOut.data(), &file.at(bmpHeader->offset), dataOut.size());
    return 0;
}

int writeFile(const std::string &filePath, void *dataIn, size_t inputSize) {
    std::ofstream fileOutputStream(filePath, std::ios::out | std::ios::binary);
    if (fileOutputStream.is_open()) {
        fileOutputStream.write((const char *)dataIn, inputSize);

        return 0;
    }
    return -1;
}

/*
 * This function translates fp16 into fp32(float) format.
 *
 * In half-precision(fp16), single-precision(fp32) & double-precision(fp64),
 * all these precision consisted of sign bit, exponent(exp), significand precision(mantis).
 * fp16: [BIT15] [BIT14 - BIT10] [BIT9  - BIT0]
 * fp32: [BIT31] [BIT30 - BIT23] [BIT22 - BIT0]
 *
 * The way precision is interpreted is different when the exponent is 0 and when it is not 0.
 * That is why there is an "if (exp != 0)" below to split the interpretation.
 *
 * It's more straightforward when exp is not 0,
 * only add 0x70 to exp due to the difference between fp16 and fp32.
 *
 * When exp is 0, 0x70 cannot be added directly to fp32,
 * instead "(v - 37) << 23" is to get required value of exp on fp32,
 * then "mantis << (150 - v)" is to get required bit shift on mantis,
 * and finally 0x7FE000 to trim it.
 */
float rawFp16ToFp32(const uint16_t &data) {
    uint32_t exp = (data & 0x7C00) >> 10;
    uint32_t mantis = (data & 0x03FF) << 13;
    uint32_t output = (data & 0x8000) << 16;

    if (exp != 0)
        output |= (exp + 0x70) << 23 | mantis;
    else if (mantis != 0) {
        auto tmp = static_cast<float>(mantis);
        uint32_t v = *reinterpret_cast<uint32_t *>(&tmp) >> 23;
        output |= (v - 37) << 23 | ((mantis << (150 - v)) & 0x7FE000);
    }

    return *reinterpret_cast<float *>(&output);
}

/*
 * This function translates bfp16 into fp32(float) format.
 * Conversion to FP32 is done by move exponent part unchanged
 * (both formats uses 8bits + sign), and extend of mantis by zeroes.
 */
float rawBfp16ToFp32(const uint16_t &data) {
    uint32_t output = (data << 16);

    return *reinterpret_cast<float *>(&output);
}

} // namespace DataHandle
