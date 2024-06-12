/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

class VpuFile {
  public:
    VpuFile(){};
    VpuFile(const std::filesystem::path &startInDir,
            const char *filename,
            const std::string &consists);

    VpuFile(const std::filesystem::path &filename) { filePath = filename; }

    template <typename T>
    bool read(T &obj) {
        std::ifstream file(filePath);

        if (!file.is_open())
            return false;

        file >> obj;

        return file.good();
    }

    template <typename T>
    bool write(T obj) {
        std::ofstream file(filePath);

        if (!file.is_open())
            return false;

        file << obj;
        /* sync with device, some tests expect that value will be rejected */
        file.flush();

        return file.good();
    }

    void set(const std::filesystem::path &filename) { filePath = filename; }

    bool accessible() {
        try {
            return std::filesystem::exists(filePath);
        } catch (std::filesystem::filesystem_error const &ex) {
            // parent directory read permission denied
            return false;
        }
    }

    int getlines(std::vector<std::string> &lines) {
        std::ifstream file(filePath);

        if (!file.is_open())
            return 0;

        for (std::string line; std::getline(file, line);)
            lines.push_back(line);

        return lines.size();
    }

    std::filesystem::path dir() { return filePath.parent_path(); }
    std::filesystem::path name() { return filePath.filename(); }
    std::filesystem::path filePath = "";
};
