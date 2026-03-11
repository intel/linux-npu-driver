/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
#include "city.h"
#include "citycrc.h"

#include <array>
#include <charconv>
#include <memory>
#include <string>

class HashCity {
  public:
    using DigestType = char *;
    static constexpr size_t HashTableSize = 5;
    using HashTable = std::array<uint64_t, HashTableSize>;
    /*Hash is composed from two parts city256 for compiled model and city64 for configuration */
    static constexpr size_t DigestLength =
        sizeof(uint64_t) * 8 + sizeof(uint64_t) * 2; // in hex representation

    HashCity() = default;
    void reset() { hash.fill(0); }

    void updateConfigurationHash(const uint8_t *data, size_t size) {
        uint64_t *configHash = &this->hash[HashTableSize - 1];
        *configHash = CityHash64WithSeed(reinterpret_cast<const char *>(data), size, *configHash);
    }
    std::string final(const uint8_t *data, size_t size) {
        CityHashCrc256(reinterpret_cast<const char *>(data), size, &hash[0]);
        return HashCity::toHexString(hash);
    }

    static std::string getDigest(const uint8_t *data, size_t size) {
        HashTable hashResult = {0};
        CityHashCrc256(reinterpret_cast<const char *>(data), size, &hashResult[0]);
        return HashCity::toHexString(hashResult);
    }

  private:
    static std::string toHexString(HashTable &value) {
        std::string str(sizeof(uint64_t) * 2 * HashTableSize, '0');

        for (size_t i = 0; i < HashTableSize; i++) {
            auto hashValue = value[i];
            auto [ptr, ec] =
                std::to_chars(str.data() + i * 16, str.data() + (i + 1) * 16, hashValue, 16);
            if (ec != std::errc()) {
                str.clear();
                break;
            }
        }
        return str;
    }

    HashTable hash = {0};
};
