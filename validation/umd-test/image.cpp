/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "image.hpp"

#ifdef UMD_TESTS_USE_OPENCV
#include <opencv2/imgcodecs.hpp>

struct Image::Impl {
    Impl(const std::string &path) {
        data = cv::imread(path);
        if (data.empty()) {
            throw "Failed to read image " + path;
        }
    }

    void *getPtr() { return data.data; }
    size_t getHeight() const { return data.size().height; }
    size_t getWidth() const { return data.size().width; }
    size_t getChannels() const { return data.channels(); }
    size_t getElementSize() const { return data.elemSize1(); }
    size_t getSizeInBytes() const { return data.total() * data.elemSize(); }

  private:
    cv::Mat data;
};

#else
#include "utilities/data_handle.h"
#include <vector>

struct __attribute__((packed)) BmpFileHeader {
    uint16_t header;
    uint32_t size;
    uint32_t reserve;
    uint32_t offset;
};

struct __attribute__((packed)) BmpInfoHeader {
    int32_t size;
    int32_t width;
    int32_t height;
    int16_t planes;
    int16_t bitCount;
    int32_t compression;
    int32_t imageSize;
    int32_t XPixelsPerMeter;
    int32_t YPixelsPerMeter;
    int32_t colorsUsed;
    int32_t colorsImportant;
};

struct Image::Impl {
    Impl(const std::string &path) {
        if (DataHandle::loadFile(path, data) != 0) {
            throw "Failed to read image " + path;
        }

        auto fileContentPtr = data.data();

        auto *bmpFileHeader = reinterpret_cast<const BmpFileHeader *>(fileContentPtr);
        auto *bmpInfoHeader =
            reinterpret_cast<const BmpInfoHeader *>(fileContentPtr + sizeof(BmpFileHeader));

        if (bmpInfoHeader->compression != 0) {
            throw "Reading compressed BMP image is not supported";
        }

        height = std::abs(bmpInfoHeader->height);
        width = bmpInfoHeader->width;
        channels = bmpInfoHeader->bitCount / 8;
        offset = bmpFileHeader->offset;

        if (channels != 3) {
            throw "Only 24 bpp is supported. This image (" + path + ") has " +
                std::to_string(bmpInfoHeader->bitCount) + " bpp";
        }

        size_t stride = width * channels;
        if (stride % 4 != 0) {
            throw "Reading image with padding is not supported";
        }

        sizeInBytes = height * stride;

        if (bmpInfoHeader->height > 0) {
            // If height > 0, pixels are laid out 'upside down'.
            // The last line is at the beginning of pixel data and the first line is at the end.
            // So we need to reverse the data in memory.
            auto dstPtr = fileContentPtr + offset;
            auto srcPtr = dstPtr + (height - 1) * stride;
            for (size_t i = 0; i < height / 2; i++) {
                for (size_t j = 0; j < stride; j++) {
                    std::swap(srcPtr[j], dstPtr[j]);
                }
                dstPtr += stride;
                srcPtr -= stride;
            }
        }
    }

    void *getPtr() { return &data[0] + offset; }
    size_t getHeight() const { return height; }
    size_t getWidth() const { return width; }
    size_t getChannels() const { return channels; }
    size_t getElementSize() const { return 1; }
    size_t getSizeInBytes() const { return sizeInBytes; }

  private:
    std::vector<char> data;
    size_t height;
    size_t width;
    size_t channels;
    size_t sizeInBytes;
    size_t offset;
};

#endif

Image::Image(const std::string &path) {
    impl = std::make_unique<Impl>(path);
}

Image::~Image() = default;

void *Image::getPtr() {
    return impl->getPtr();
}

size_t Image::getHeight() const {
    return impl->getHeight();
}

size_t Image::getWidth() const {
    return impl->getWidth();
}

size_t Image::getChannels() const {
    return impl->getChannels();
}

size_t Image::getElementSize() const {
    return impl->getElementSize();
}

size_t Image::getSizeInBytes() const {
    return impl->getSizeInBytes();
}
