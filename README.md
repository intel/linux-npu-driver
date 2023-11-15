<!---

Copyright (C) 2022-2023 Intel Corporation

SPDX-License-Identifier: MIT

-->

# Intel(R) AI Boost

## Introduction

The Intel(R) AI Boost is an AI inference accelerator integrated with Intel
non-server CPUs starting from 14th generation. It enables energy efficient
execution of Deep Learning applications like object detection, classification etc.

This type of device is called a Neural Processing Unit (NPU), and the Linux kernel
uses the name Versatile Processing Unit (VPU).

### Build standalone driver

Install required dependencies in Ubuntu

```
sudo apt update
sudo apt install -y build-essential git git-lfs cmake libudev-dev libboost-all-dev
```

Commands to build driver

```
cd linux-npu-driver
cmake -B build -S .
cmake --build build --parallel $(nproc)
```

The driver can be found in `build/lib/libze_intel_vpu.so`. The user can link
directly to driver or through the Level Zero loader that is built together with
driver and can be found in `build/lib/libze_loader.so` path.

## Build driver together with compiler

The driver has a target in `compiler/vpux_compiler.cmake` to build
Compiler-in-Driver component from [NPU plugin
repository](https://github.com/openvinotoolkit/npu_plugin.git).

OpenVINO runtime is required by compiler. About the dependencies for building OpenVINO,
please check the [OpenVINO build
document](https://github.com/openvinotoolkit/openvino/blob/master/docs/dev/build.md)

*Note*: CMake version 3.22 or higher is required

To build compiler from driver repository the `ENABLE_VPUX_COMPILER_BUILD` flag has to be set
```
cd linux-npu-driver
cmake -B build -S . -DENABLE_VPUX_COMPILER_BUILD=ON
cmake --build build --parallel $(nproc)
```

The compiler binary can be found in `build/lib/libvpux_driver_compiler.so`

## Components

The repository collects components that are required to work with Intel(R) AI Boost
in Linux systems.

```
linux-npu-driver/
| # Compiler-in-Driver build recipes
├─ compiler/
| # Firmware binary and headers
├─ firmware/
| # Linux kernel user space API headers
├─ linux/include/uapi
| # User Mode Driver source code
└─ umd/
```

### User Mode Driver (UMD)

UMD allows user to get information about device and push workload to kernel
driver. UMD implements Level Zero API Driver that standardize user interface.
Driver implements the [Graph
Extension](https://github.com/intel/level-zero-npu-extensions.git) as part of Level
Zero API that allows to compile IR model and run inference in NPU.

### Kernel Mode Driver (KMD)

Linux kernel driver for NPU hardware comes with user space API header.
This header allows the UMD to communicate with NPU and submit inference
using ioctl functions.

The kernel driver `intel_vpu` is available from kernel 6.4 and can be found
in the driver/accel/ivpu tree of Linux Kernel repository.

https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/accel/ivpu

### Firmware binary and headers

Firmware is required for NPU hardware to manage its internal hardware
components. Firmware co-operates with kernel driver to boot up all NPU
hardware components and manages the user workload. Binary has to be
installed into directory that is accessible by kernel driver

```
cp firmware/bin/vpu_37xx_v0.0.bin /lib/firmware/intel/vpu/
```

### Compiler-in-Driver

Compiler source code is placed in [NPU plugin
repository](https://github.com/openvinotoolkit/npu_plugin.git). Driver library
requires a compiler to fully enable the functionality of the Graph extension.
Without the compiler, the driver can only push precompiled models.

### NPU plugin

[NPU plugin repository](https://github.com/openvinotoolkit/npu_plugin.git) stores
the compiler code together with various tools for NPU device. Thanks to NPU
plugin user can access NPU device using OpenVINO API.

The NPU plugin is included in the official OpenVINO Archives distribution
since OpenVINO 2023.1.

## License

The Intel(R) AI Boost Driver is distributed under the MIT License.

You may obtain a copy of the License at: https://opensource.org/licenses/MIT

## See also
* [oneAPI Level Zero specification](https://spec.oneapi.io/level-zero/1.6.0/index.html)
* [Intel(R) OneApi Level Zero Specification API C/C++ header files](https://github.com/oneapi-src/level-zero/)
* [OpenVINO documentation](https://docs.openvino.ai/2023.0/home.html)
* [OpenVINO repository](https://github.com/openvinotoolkit/openvino.git)
* [NPU plugin - compiler](https://github.com/openvinotoolkit/npu_plugin.git)
