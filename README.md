<!---

Copyright (C) 2022-2023 Intel Corporation

SPDX-License-Identifier: MIT

-->

# Intel NPU

## Introduction

The Intel NPU is an AI inference accelerator integrated with Intel client CPUs,
starting from its 14th generation - Meteor Lake (also known as Intel Core Ultra).
It enables energy-efficient execution of Deep Learning models like object detection,
classification etc.

The full device name is Neural Processing Unit, but the Linux kernel driver uses
the older name - Versatile Processing Unit (VPU).

### Driver releases

The release contains binaries targeted for specific system and list of components
that has been used for testing.

[Go to release page](https://github.com/intel/linux-npu-driver/releases)

### Build standalone driver

Install required dependencies in Ubuntu

```
sudo apt update
sudo apt install -y build-essential git git-lfs cmake libudev-dev libboost-all-dev
```

Commands to build driver

```
cd linux-npu-driver
git submodule update --init --recursive
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

The repository collects components that are required to work with Intel NPU
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
| # Test application
└─ validation/
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

*Note*: The NPU plugin is included in the official OpenVINO Archives distribution
since OpenVINO 2023.1.
([OpenVINO 2023.1](https://docs.openvino.ai/2023.1/openvino_docs_install_guides_overview.html?VERSION=v_2023_1_0&OP_SYSTEM=LINUX&DISTRIBUTION=ARCHIVE),
[OpenVINO 2023.2](https://docs.openvino.ai/2023.2/openvino_docs_install_guides_overview.html?VERSION=v_2023_2_0&OP_SYSTEM=LINUX&DISTRIBUTION=ARCHIVE))

## Driver test application

The `validation/umd-test` directory contains `vpu-umd-test` application with functional tests.
This application allows to configure test content thanks to YAML config.
Information about YAML config can be found in [validation/umd-test/configs](/validation/umd-test/configs)

The binary `vpu-umd-test` is located in the build folder, ex. `build/bin/`

How to run:
```
./vpu-umd-test --config=basic.yaml
```

## FAQ

* Non-root access to the NPU device

To access the NPU device the user needs to be in "render" or "video" group.
Group depends on system configuration

```
# Check user groups
groups

# Add user to render group
sudo usermod -a -G render <user-name>

# Log out and log in to apply new group
```

The patch for systemd to set "render" group for accel subsystem has been merged
but might not be available in your Linux distribution. See
[systemd change](https://github.com/systemd/systemd/pull/27785)

If setting "render" group does not fix non-root access issue, admin needs to
set group manually

```
# Check device permissions
ls -l /dev/accel/

# Change group for accel device
sudo chown root:render /dev/accel/accel0

# Allow to read/write from device for group
chmod g+rw /dev/accel/accel0

# Check final permissions
$ ls -lah /dev/accel/accel0
crw-rw---- 1 root render 261, 0 Jan 31 15:58 /dev/accel/accel0
```

* Compilation issue

The compilation may fail due to memory shortage. The recommendation is to
use Ninja generator instead of Unix Makefiles. If it won't help, please
[file a new issue](https://github.com/intel/linux-npu-driver/issues/new)

```
# Install Ninja
sudo apt update
sudo apt install -y ninja-build

# Remove old build and create new one
rm build -rf
cmake -B build -S . -G Ninja
```

## License

The Intel NPU driver is distributed under the MIT License.

You may obtain a copy of the License at: https://opensource.org/licenses/MIT

## See also
* [OpenVINO documentation](https://docs.openvino.ai/2023.0/home.html)
* [OpenVINO repository](https://github.com/openvinotoolkit/openvino.git)
* [NPU plugin - compiler](https://github.com/openvinotoolkit/npu_plugin.git)
* [oneAPI Level Zero specification](https://spec.oneapi.io/level-zero/1.6.0/index.html)
* [Intel(R) OneApi Level Zero Specification API C/C++ header files](https://github.com/oneapi-src/level-zero/)
