/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

inline bool file_exists(const fs::path &path) {
    std::error_code ec;
    return fs::exists(path, ec);
}

template <class T>
inline int write_file(const fs::path &path, const T &val) {
    std::ofstream file(path, std::ios::out | std::ios::binary);

    if (!file) {
        return errno;
    }

    file << val;
    if (!file) {
        return errno;
    }

    file.close();
    if (!file) {
        return errno;
    }

    return 0;
}

template <class T>
inline void write_file_or_throw(const fs::path &path, const T &val) {
    if (int ret = write_file(path, val))
        throw std::runtime_error("Error writing " + path.string() + ": " + std::strerror(ret));
}

template <class T>
inline int write_existing_file(const fs::path &path, const T &val) {
    if (!file_exists(path))
        return ENOENT;
    return write_file(path, val);
}

inline int read_file_to_ss(const fs::path &path, std::stringstream &ss) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file)
        return errno;

    ss << file.rdbuf();
    if (!file) {
        return errno;
    }

    return 0;
}

template <class T>
inline int read_file(const fs::path &path, T &val) {
    std::stringstream ss;

    int ret = read_file_to_ss(path, ss);
    if (ret)
        return ret;

    if (std::is_integral_v<T> && ss.str().substr(0, 2) == "0x")
        ss >> std::hex >> val;
    else
        ss >> val;

    return ss.fail() ? EINVAL : 0;
}

template <>
inline int read_file<std::string>(const fs::path &path, std::string &str) {
    std::stringstream ss;

    int ret = read_file_to_ss(path, ss);
    if (ret)
        return ret;

    str = ss.str();
    int lines = std::count(str.begin(), str.end(), '\n');
    while (lines == 1 && !str.empty() && str.back() == '\n') {
        str.pop_back(); // remove trailing newlines in a single line files
    }

    return 0;
}

template <class T>
inline void read_file_or_throw(const fs::path &path, T &val) {
    if (int ret = read_file(path, val))
        throw std::runtime_error("Error reading " + path.string() + ": " + std::strerror(ret));
}

inline int read_lines(const fs::path &path, std::vector<std::string> &lines) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file)
        return errno;

    std::string line;
    while (std::getline(file, line)) {
        if (file.bad()) {
            return errno;
        }
        lines.push_back(line);
    }

    if (!file.eof())
        return errno;

    return 0;
}