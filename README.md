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

## NPU software stack

```
 ──[User─space]──────────────────────────────────────────────────────────────────────────────
                                                                                             
     ┌──────────────────────────(optional)─┐   ┌────────────────────────────────────────┐    
     │ User Mode Driver tests              │   │ OpenVINO                               │    
     │                                     │   │                                        │    
     │         intel-validation-npu        │   │            openvino_toolkit            │    
     │            (vpu-umd-test)           │   │                                        │    
     │                                     │   │                                        │    
     └──────────────────▲──────────────────┘   └────────────────────▲───────────────────┘    
                        ╚════════════════════╦══════════════════════╝                        
 ............................................║.............................................. 
     ┌───────────────────────────────────────▼──────────────────────────────────────────┐    
     │ oneAPI Level-Zero                 level-zero                                     │    
     │                                (libze_loader.so)                                 │    
     │                                                                                  │    
     └───────────────────────────────────────▲──────────────────────────────────────────┘    
                        ╔════════════════════╝                                               
     ┌──────────────────▼──────────────────┐   ┌────────────────────────────────────────┐    
     │ User Mode Driver                    │   │ NPU compiler                           │    
     │                                     │   │                                        │    
     │        intel-level-zero-npu         ◀═══▶     intel-driver-compiler-npu         │    
     │        (libze_intel_vpu.so)         │   │     (libvpux_driver_compiler.so)       │    
     │                                     │   │                                        │    
     └──────────────────▲──────────────────┘   └────────────────────────────────────────┘    
                        ╚════════════════════╗                                               
 ──[Kernel─space]────────────────────────────║───────────────────────────────────────────────
                         ┌───────────────────▼─────────────────────┐                         
                         │ Kernel Mode Driver                      │                         
                         │            intel-npu-driver             │                         
                         │             (intel_vpu.ko)              │                         
                         │                                         │                         
                         └───────────────────▲─────────────────────┘                         
 ──[Device]──────────────────────────────────║────────────────────────────────────────────── 
                         ┌───────────────────▼─────────────────────┐                         
                         │ Firmware                                │                         
                         │             intel-fw-npu                │                         
                         │                                         │                         
                         └─────────────────────────────────────────┘                         
```

## Building a standalone driver

Install the required dependencies in Ubuntu:
```
sudo apt update
sudo apt install -y build-essential git git-lfs cmake libudev-dev libboost-all-dev
```

Commands to build the driver:
```
cd linux-npu-driver
git submodule update --init --recursive
cmake -B build -S .
cmake --build build --parallel $(nproc)

# set the LD_LIBRARY_PATH to lib to make driver visible for loader
export LD_LIBRARY_PATH=$PWD/build/lib:$LD_LIBRARY_PATH
# or install the driver in the system
cmake --install build
```

## Building a driver together with the compiler

The driver has a target in `compiler/vpux_compiler.cmake` to build
Compiler-in-Driver component from [NPU plugin
repository](https://github.com/openvinotoolkit/npu_plugin.git).

OpenVINO runtime is required by compiler. About the dependencies for building OpenVINO,
please check the [OpenVINO build
document](https://github.com/openvinotoolkit/openvino/blob/master/docs/dev/build.md).

To build a compiler from the driver repository the `ENABLE_VPUX_COMPILER_BUILD` flag has to be set:
```
cd linux-npu-driver
cmake -B build -S . -DENABLE_VPUX_COMPILER_BUILD=ON
cmake --build build --parallel $(nproc)

# set the LD_LIBRARY_PATH to lib to make driver visible for loader
export LD_LIBRARY_PATH=$PWD/build/lib:$LD_LIBRARY_PATH
# or install the driver in the system
cmake --install build
```

The compiler binary `libvpux_driver_compiler.so` can be found in `build/lib/`.

## Driver test application

The `validation/umd-test` directory contains `vpu-umd-test` application with functional tests.
This application allows to configure test content using YAML configuration file.

More information can be found in [validation/umd-test/configs](/validation/umd-test/configs).

The binary `vpu-umd-test` is located in the build folder, ex. `build/bin/`

Command line:
```
./vpu-umd-test --config=basic.yaml
```

## FAQ

<details>
<summary>Non-root access to the NPU device</summary>

To access the NPU device the user needs to be in "render" or "video" group.
A group depends on system configuration:
```
# check user groups
groups

# add user to the "render" group
sudo usermod -a -G render <user-name>

# log out and log in to apply the new group
```

The patch for systemd to set he "render" group for accel subsystem has been merged
but might not be available in your Linux distribution. See
[systemd change](https://github.com/systemd/systemd/pull/27785).

If setting the "render" group does not resolve the non-root access issue,
this must be done by an administrator manually:
```
# check device permissions
ls -l /dev/accel/

# change group for accel device
sudo chown root:render /dev/accel/accel0

# allow to read/write from device for group
sudo chmod g+rw /dev/accel/accel0

# check final permissions
$ ls -lah /dev/accel/accel0
crw-rw---- 1 root render 261, 0 Jan 31 15:58 /dev/accel/accel0
```
</details>
<details>
<summary>Compilation problem due to lack of memory</summary>

The compilation may fail due to memory shortage. The recommendation is to
use the Ninja generator instead of Unix Makefiles. If it does not help, please
[file a new issue](https://github.com/intel/linux-npu-driver/issues/new).

```
# install Ninja
sudo apt update
sudo apt install -y ninja-build

# remove the old build and create a new one
rm build -rf
cmake -B build -S . -G Ninja
```
</details>
<details>
<summary>Enable the intel_vpu module in the kernel config for compilation</summary>

In the Kernel Menu Configuration from `Device Driver` select `Compute Acceleration Framework`
and set "modularize" for `Intel NPU (Neural Processing Unit)`.
</details>
<details>
<summary>Finding the intel_vpu kernel module in the system</summary>

```
# check if the intel_vpu exists is in the system
modinfo intel_vpu

# check if the intel_vpu is loaded in the kernel
lsmod | grep intel_vpu

# if the previous command nothing produced, load the intel_vpu
sudo modprobe intel_vpu

# verify that the intel_vpu has been loaded successfully
sudo dmesg | tail -n 20
```
</details>
<details>
<summary>Enable driver log using an environment variable</summary>

Valid logging levels are `ERROR`, `WARNING`, `INFO`, `VERBOSE`.

Seting the logging level using the `ZE_INTEL_NPU_LOGLEVEL` environment variable:
```
export ZE_INTEL_NPU_LOGLEVEL=<logging_level>
```

Command to clear an exported value:
```
unset ZE_INTEL_NPU_LOGLEVEL
```
</details>

## License

Intel® NPU driver is distributed under the MIT License.

You may obtain a copy of the license at: https://opensource.org/licenses/MIT

## See also
* [OpenVINO documentation](https://docs.openvino.ai/2024/home.html)
* [OpenVINO repository](https://github.com/openvinotoolkit/openvino.git)
* [NPU plugin - compiler](https://github.com/openvinotoolkit/npu_plugin.git)
* [Intel(R) OneApi Level Zero Specification API C/C++ header files](https://github.com/oneapi-src/level-zero/)
* [Running NPU tasks using Intel® NPU Acceleration Library](https://intel.github.io/intel-npu-acceleration-library/)
