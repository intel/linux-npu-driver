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
    using DigestType = char *;
    static constexpr size_t DigestLength = SHA1_DIGEST_LENGTH * 2;

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

    static std::string getDigest(const uint8_t *data, size_t size) {
        HashSha1 hash;
        hash.update(data, size);
        return hash.final();
    }

  private:
    SHA1_CTX context;
};
