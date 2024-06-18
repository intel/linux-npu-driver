<!---

Copyright (C) 2022-2024 Intel Corporation

SPDX-License-Identifier: MIT

-->

# Intel® NPU

## Introduction

This repository contains User Mode Driver for Intel® NPU device.

Intel® NPU device is an AI inference accelerator integrated with Intel client CPUs,
starting from Intel® Core™ Ultra generation of CPUs (formerly known as Meteor Lake).
It enables energy-efficient execution of artificial neural network tasks.

To make sure that your system has an NPU available, please follow
[the steps](https://www.intel.com/content/www/us/en/support/articles/000097597/processors.html).

The full device name is Neural Processing Unit, but the Linux kernel driver uses
the older name - Versatile Processing Unit (VPU).

## Driver releases

[The release page contains the binaries and supported component versions](https://github.com/intel/linux-npu-driver/releases).

## Build driver and common issues

[The docs directory contains overiew.md that explains how to build driver and common issues](docs/overview.md)

## License

Intel® NPU driver is distributed under the MIT License.

You may obtain a copy of the license at: https://opensource.org/licenses/MIT

## See also

* [OpenVINO™ documentation](https://docs.openvino.ai/2024/home.html)
* [OpenVINO™ repository](https://github.com/openvinotoolkit/openvino.git)
* [NPU plugin - compiler](https://github.com/openvinotoolkit/npu_plugin.git)
* [Intel® OneApi Level Zero Specification API C/C++ header files](https://github.com/oneapi-src/level-zero/)
* [Running NPU tasks using Intel® NPU Acceleration Library](https://intel.github.io/intel-npu-acceleration-library/)
* [OpenVINO™ AI Plugins for GIMP](https://github.com/intel/openvino-ai-plugins-gimp)
* [OpenVINO™ AI Plugins for Audacity*](https://github.com/intel/openvino-plugins-ai-audacity)
* [OpenVINO™ Plugins for OBS Studio](https://github.com/intel/openvino-plugins-for-obs-studio)
