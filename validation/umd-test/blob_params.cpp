/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "blob_params.hpp"
#include "utilities/data_handle.h"
#include "test_app.h"

#include <gtest/gtest.h>

#include <string.h>

bool loadBlobDataFromNode(const std::string &blobPath,
                          std::vector<std::string> inputFiles,
                          std::vector<std::string> outputFiles,
                          std::vector<std::vector<char>> &inputBin,
                          std::vector<std::vector<char>> &outputBin) {
    std::filesystem::path blobFile(blobPath);
    std::filesystem::path dataDir = blobFile.parent_path();

    inputBin.resize(inputFiles.size());
    for (size_t i = 0; i < inputBin.size(); i++) {
        auto inputPath = dataDir / inputFiles[i];
        TRACE("Input file: %s\n", inputPath.c_str());
        if (DataHandle::loadFile(inputPath, inputBin[i]) != 0) {
            PRINTF("Failed to load input from %s\n", inputPath.c_str());
            return false;
        }
    }

    outputBin.resize(outputFiles.size());
    for (size_t i = 0; i < outputBin.size(); i++) {
        auto outputPath = dataDir / outputFiles[i];
        TRACE("Output file: %s\n", outputPath.c_str());
        if (DataHandle::loadFile(outputPath, outputBin[i]) != 0) {
            PRINTF("Failed to load output from %s\n", outputPath.c_str());
            return false;
        }
    }

    return true;
}

bool loadBlobFromPath(const std::string &blobPath, std::vector<char> &npuBlob) {
    std::filesystem::path blobFile(blobPath);

    TRACE("Blob: %s\n", blobFile.c_str());
    if (DataHandle::loadFile(blobFile, npuBlob) != 0) {
        PRINTF("Failed to load blob from %s\n", blobFile.c_str());
        return false;
    }

    return true;
}
