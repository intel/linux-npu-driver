/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "file_helper.h"

VpuFile::VpuFile(const std::filesystem::path &startInDir,
                 const char *filename,
                 const std::string &consists) {
    filePath.clear();
    for (const std::filesystem::directory_entry &entry :
         std::filesystem::recursive_directory_iterator(startInDir)) {
        std::ifstream file(entry.path() / filename, std::ios::binary);
        if (!file.is_open())
            continue;

        std::string content;
        file >> content;
        if (content == consists) {
            filePath = entry.path() / filename;
            break;
        }
    }
}
