/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <openssl/evp.h>
#include <openssl/sha.h>

#include <array>
#include <charconv>
#include <memory>
#include <string>

class HashSha1 {
  public:
    HashSha1()
        : pEvpMdCtx(EVP_MD_CTX_create(), &EVP_MD_CTX_free) {
        EVP_DigestInit_ex(pEvpMdCtx.get(), EVP_sha1(), nullptr);
    }

    void update(const uint8_t *data, size_t size) { EVP_DigestUpdate(pEvpMdCtx.get(), data, size); }

    std::string final() {
        std::array<uint8_t, SHA_DIGEST_LENGTH> value;
        EVP_DigestFinal_ex(pEvpMdCtx.get(), value.data(), nullptr);

        constexpr auto byteToHexLetters = 2;
        std::string str(SHA_DIGEST_LENGTH * byteToHexLetters, '0');
        for (size_t i = 0; i < value.size(); i++) {
            std::to_chars(&str[i * byteToHexLetters],
                          &str[i * byteToHexLetters + byteToHexLetters],
                          value[i],
                          16);
        }
        return str;
    }

  private:
    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> pEvpMdCtx;
};
