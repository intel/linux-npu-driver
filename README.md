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

The repository collects components that are required to work with Intel(R)
Versatile Processing Unit in Linux systems.

## Components

```
linux-vpu-driver/
| # OpenVINO+VPUX plugin package and Compiler-in-driver build recipes
├─ compiler/
| # Firmware binary and headers
├─ firmware/
| # Linux kernel user space API headers
├─ linux/include/uapi
| # User-mode driver source code
└─ umd/
```

### User-Mode driver (UMD)

UMD allows user to get information about device and push workload to kernel
driver. UMD implements Level Zero API Driver that standardize user interface.
Driver implements the [Graph
Extension](https://github.com/intel/level-zero-vpu-extensions) as part of Level
Zero API that allows to compile IR model and run inference in VPU.

### Linux User Space API (UAPI)

Linux kernel driver for VPU hardware comes with user space API header. This
header allows the user-mode driver to communicate with VPU and submit inference
using ioctl function.

### Firmware binary and headers

Firmware is required for VPU hardware to manage its internal hardware
components. Firmware co-operates with kernel driver to boot up all VPU
hardware components and manages the user workload. Binary has to be
installed into directory that is accessible by kernel driver

```
cp firmware/bin/mtl_vpu_v0.0.bin /lib/firmware/intel/vpu/
```

### Compiler-in-Driver

Compiler source code is placed in [VPUX plugin
repository](https://github.com/openvinotoolkit/vpux-plugin). The driver library
requires compiler to fully enabled Graph extension functionality.
Driver links to compiler-in-driver and user can access it through Graph
extension. Without the compiler the driver can only push precompiled models.

### VPUX plugin

[VPUX plugin repository](https://github.com/openvinotoolkit/vpux-plugin) stores
the compiler code together with various tools for VPU device. Thanks to VPUX
plugin user can access VPU device using OpenVINO API.

## Kernel driver - intel_vpu

The kernel driver can be found in the driver/accel/ivpu tree of Linux Kernel
repository.

https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/accel/ivpu

## Build standalone driver

Install required dependencies in Ubuntu

```
sudo apt update
sudo apt install -y build-essential git cmake libudev-dev libboost-all-dev
```

Commands to build driver

```
cd linux-vpu-driver
cmake -Bbuild -S . -GNinja
cmake --build build --parallel $(nproc)
```

The driver can be found in `build/lib/libze_intel_vpu.so`. The user can link
directly to driver or through the Level Zero loader that is built together with
driver and can be found in `build/lib/libze_loader.so` path.

## Build driver together with compiler

The driver has a target in `umd/cmake/vpux_compiler.cmake` to build
Compiler-in-Driver component from [VPUX plugin
repository](https://github.com/openvinotoolkit/vpux_plugin.git).

The compiler required to build OpenVINO runtime and is built as OpenVINO
plugin. About the dependencies for building OpenVINO, please, check the
[OpenVINO build
document](https://github.com/openvinotoolkit/openvino/blob/master/docs/dev/build.md)

*Note*: CMake version 3.22 or higher is required

To build compiler from driver repository the `ENABLE_VPUX_COMPILER` flag has to be set
```
cd linux-vpu-driver
cmake -B build -S . -GNinja -DENABLE_VPUX_COMPILER=ON
cmake --build build --parallel $(nproc)
```

The compiler binary can be found in `build/lib/libVPUXCompilerL0.so`

## License

The Intel(R) Versatile Processing Unit Driver is distributed under the MIT License.

You may obtain a copy of the License at: https://opensource.org/licenses/MIT

## See also

### Level Zero specific
* [oneAPI Level Zero specification](https://spec.oneapi.com/level-zero/latest/index.html)
* [Intel(R) OneApi Level Zero Specification API C/C++ header files](https://github.com/oneapi-src/level-zero/)
* [oneAPI Level Zero tests](https://github.com/oneapi-src/level-zero-tests/)
* [OpenVINO documentation](https://docs.openvino.ai/2023.0/home.html)
* [OpenVINO repository](https://github.com/openvinotoolkit/openvino.git)
* [VPUX plugin - compiler](https://github.com/openvinotoolkit/vpux_plugin.git)
