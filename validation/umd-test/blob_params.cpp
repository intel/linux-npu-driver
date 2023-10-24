/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"
#include "blob_params.hpp"
#include "test_app.h"

#include <gtest/gtest.h>

#include <string.h>

bool getBlobFromPath(std::string blobPath,
                     std::vector<std::string> inputFiles,
                     std::vector<std::string> outputFiles,
                     std::vector<char> &vpuBlob,
                     std::vector<std::vector<char>> &inputBin,
                     std::vector<std::vector<char>> &outputBin,
                     std::vector<char> &actShavesBin) {
    std::filesystem::path blobFile(blobPath);
    std::filesystem::path dataDir = blobFile.parent_path();
    std::filesystem::path actShavesFile(blobPath);

    actShavesFile.replace_extension(".bin");

    if (!UmdTest::loadFile(blobFile, vpuBlob)) {
        PRINTF("Failed to load blob from %s\n", blobFile.c_str());
        return false;
    }

    if (std::filesystem::exists(actShavesFile)) {
        if (!UmdTest::loadFile(actShavesFile, actShavesBin)) {
            PRINTF("Failed to load activation shave binary from %s\n", actShavesFile.c_str());
            return false;
        }
    }

    inputBin.resize(inputFiles.size());
    for (size_t i = 0; i < inputBin.size(); i++) {
        auto inputPath = dataDir / inputFiles[i];
        if (!UmdTest::loadFile(inputPath, inputBin[i])) {
            PRINTF("Failed to load input from %s\n", inputPath.c_str());
            return false;
        }
    }

    outputBin.resize(outputFiles.size());
    for (size_t i = 0; i < outputBin.size(); i++) {
        auto outputPath = dataDir / outputFiles[i];
        if (!UmdTest::loadFile(outputPath, outputBin[i])) {
            PRINTF("Failed to load output from %s\n", outputPath.c_str());
            return false;
        }
    }

    return true;
}
