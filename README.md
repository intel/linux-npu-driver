<!---

Copyright (C) 2022 Intel Corporation

SPDX-License-Identifier: MIT

-->

# Intel(R) Versatile Processing Unit Driver

## Introduction

The Intel(R) Versatile Processing Unit (VPU) is an AI inference accelerator
integrated with Intel non-server CPUs starting from 14th generation. VPU
enables energy efficient execution of Deep Learning applications like object
detection, classification etc.

The repository contains components that are required to work with Intel(R)
Versatile Processing Unit in Linux systems.

## Components

# UMD

User-mode driver (UMD) allows user to get information about device and push
workload to kernel driver. UMD implements Level Zero API Driver that standardize
user interface.

## Building

UMD driver depends on OpenVino and VPUX plugin, recommended configuration for
building is: Ubuntu20.04 and cmake version 3.22 (need to be upgraded from
default Ubuntu20.04 cmake version).
To build driver execute:
mkdir build
cd build
cmake ../
make -j`nproc`

## License

The Intel(R) Versatile Processing Unit Driver is distributed under the MIT License.

You may obtain a copy of the License at: https://opensource.org/licenses/MIT

## See also

### Level Zero specific
* [oneAPI Level Zero specification](https://spec.oneapi.com/level-zero/latest/index.html)
* [Intel(R) OneApi Level Zero Specification API C/C++ header files](https://github.com/oneapi-src/level-zero/)
* [oneAPI Level Zero tests](https://github.com/oneapi-src/level-zero-tests/)
