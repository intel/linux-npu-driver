/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"
#include "model_params.hpp"
#include "test_app.h"

#include <gtest/gtest.h>

#include <string.h>

bool getModelFromPath(std::string modelXmlPath, std::vector<char> &xml, std::vector<char> &bin) {
    std::filesystem::path modelXml(modelXmlPath);
    std::filesystem::path modelBin(modelXmlPath);

    modelBin.replace_extension(".bin");

    if (std::filesystem::exists(modelXml) == false || std::filesystem::exists(modelBin) == false) {
        PRINTF("Model %s doesn't exist.\n", modelXml.c_str());
        return false;
    }

    if (!UmdTest::loadFile(modelXml, xml)) {
        PRINTF("Failed to load model from %s\n", modelXml.c_str());
        return false;
    }
    if (!UmdTest::loadFile(modelBin, bin)) {
        PRINTF("Failed to load weights from %s\n", modelBin.c_str());
        return false;
    }

    if (xml.size() == 0)
        return false;

    return true;
}
