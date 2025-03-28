/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "sha1.h"

#include <array>
#include <charconv>
#include <memory>
#include <string>

class HashSha1 {
  public:
    HashSha1() { SHA1Init(&context); }

    void update(const uint8_t *data, size_t size) { SHA1Update(&context, data, size); }

    std::string final() {
        std::array<uint8_t, SHA1_DIGEST_LENGTH> value;
        SHA1Final(value.data(), &context);

        static const char *hexChars = "0123456789abcdef";
        constexpr auto byteToHexLetters = 2;
        std::string str(SHA1_DIGEST_LENGTH * byteToHexLetters, '0');
        for (size_t i = 0; i < value.size(); i++) {
            str[byteToHexLetters * i] = hexChars[value[i] >> 4];
            str[byteToHexLetters * i + 1] = hexChars[value[i] & 0xf];
        }
        return str;
    }

  private:
    SHA1_CTX context;
};
