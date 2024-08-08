/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "vpu_driver/source/utilities/log.hpp"

#include <filesystem>
#include <getopt.h>
#include <stdio.h>
#include <string>
#include <unistd.h>

namespace TestOptions {

std::filesystem::path blobPath;
std::filesystem::path modelPath;
std::string modelFlags;

} // namespace TestOptions

static void printHelpMessage() {
    constexpr auto helpMessage =
        "\n"
        "Extra test options:\n"
        "  -v/--verbose            Set log level to verbose\n"
        "  -l/--loglevel <lvl>     Set log level - error, warning, info, verbose\n"
        "  -b/--blob <path>        Path to blob\n"
        "  -m/--model <path>       Path to model\n"
        "  -c/--model_flags <cfg>  Model build flags\n"
        "\n";

    printf("%s\n", helpMessage);
}

static bool parseOptions(int argc, char **argv) {
    static struct option longOptions[] = {{"verbose", no_argument, 0, 'v'},
                                          {"loglevel", required_argument, 0, 'l'},
                                          {"blob", required_argument, 0, 'b'},
                                          {"model", required_argument, 0, 'm'},
                                          {"model_flags", required_argument, 0, 'c'},
                                          {}};

    int opt;
    while ((opt = getopt_long(argc, argv, "vl:b:m:c:", longOptions, nullptr)) != -1) {
        switch (opt) {
        case 'v':
            VPU::setLogLevel(INFO);
            break;
        case 'l':
            VPU::setLogLevel(optarg);
            break;
        case 'b':
            TestOptions::blobPath = optarg;
            break;
        case 'm':
            TestOptions::modelPath = optarg;
            break;
        case 'c':
            TestOptions::modelFlags = optarg;
            break;
        default:
            printHelpMessage();
            return false;
        }
    }
    return true;
}

static void setDefaultOptions() {
    const std::filesystem::path defaultBlob = "validation/blobs/mtl/mobilenet-v2/vpuip.blob";
    if (TestOptions::blobPath.empty() && std::filesystem::exists(defaultBlob)) {
        TestOptions::blobPath = defaultBlob;
    }

    const std::filesystem::path defaultModel = "validation/models/mtl/add_abc/add_abc.xml";
    if (TestOptions::modelPath.empty() && std::filesystem::exists(defaultModel)) {
        TestOptions::modelPath = defaultModel;
        TestOptions::modelFlags =
            "--inputs_precisions=\"A:fp16 B:fp16 C:fp16\" --inputs_layouts=\"A:C B:C C:C\" "
            "--outputs_precisions=\"Y:fp16\" --outputs_layouts=\"Y:C\"";
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleMock(&argc, argv);
    if (!parseOptions(argc, argv))
        return 1;
    setDefaultOptions();
    return RUN_ALL_TESTS();
}
