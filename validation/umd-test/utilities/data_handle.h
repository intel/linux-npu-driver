/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <linux/kernel.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace DataHandle {

/**
    Fill data with random values
    @param size[in]: Size of data
    @param data[out]: Reference to vector to store data
  */
void generateRandomData(std::vector<char> &data, size_t size);

/**
    Load a binary file into the program
    @param filePath[in]: Pointer to string containing filepath for file to be loaded.
    @param dataOut[out]: Reference to vector to store raw data.
    @return : int 0 indicating success, -1 if otherwise
  */
int loadFile(const std::string &filePath, std::vector<char> &dataOut);

/**
    Load an image file into the program
    @param filePath[in]: Pointer to string containing filepath for file to be loaded.
    @param dataOut[out]: Reference to vector to store raw data.
    @return : int 0 indicating success, -1 if otherwise
  */
int loadImageFile(const std::string &filePath, std::vector<char> &dataOut);

/**
    Write data out to a file
    @param filePath[in]: Pointer to string containing filepath where results would be stored in.
    @param dataIn[in]: Reference to vector to obtain data for storing.
    @param inputSize[in]: size of data to be written from void pointer
    @return : int 0 indicating success, -1 if otherwise
  */
int writeFile(const std::string &filePath, void *dataIn, size_t inputSize);

/**
    Take reference of raw FP16 data and convert into float(FP32)
    @param data[in]: Reference to raw FP16 to convert into float(FP32).
  */
float rawFp16ToFp32(const uint16_t &data);

/**
    Take reference of raw BFP16 data and convert into float(FP32)
    @param data[in]: Reference to raw BFP16 to convert into float(FP32).
  */
float rawBfp16ToFp32(const uint16_t &data);
} // namespace DataHandle
