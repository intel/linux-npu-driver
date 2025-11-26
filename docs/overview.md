<!---

Copyright (C) 2022-2025 Intel Corporation

SPDX-License-Identifier: MIT

-->

## NPU software stack

```
 ──[User─space]──────────────────────────────────────────────────────────────────────────────
                                                                                             
     ┌──────────────────────────(optional)─┐   ┌────────────────────────────────────────┐    
     │ User Mode Driver tests              │   │ OpenVINO + NPU plugin                  │    
     │                                     │   │                                        │    
     │         intel-validation-npu        │   │            openvino_toolkit            │    
     │            (npu-umd-test)           │   │                                        │
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
     │        (libze_intel_npu.so)         │   │     (libnpu_driver_compiler.so)        │    
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

## Changelog

<details>
<summary>Driver library name change from libze_intel_vpu.so to libze_intel_npu.so (from v1.16.0)</summary>

Starting from v1.16.0 release the driver library name has been changed from `libze_intel_vpu.so` to
`libze_intel_npu.so`. The old library name is still supported for backward compatibility in Level
Zero loader, but it is recommended to use the new library name. Using an older version of Level Zero
than v1.17.17 requires to keep the old library name.

</details>

<details>
<summary>zeMutableCommandList extension implementation (from v1.6.0)</summary>

Implemented zeMutableCommandList extension from Level Zero API in v1.6.0 driver
release. This feature allows to change arguments of inference recorded in
zeCommandList by zeGraph->appendGraphExecute call. Using zeMutableCommandList
user can easily exchange argument of Graph Execute without recording new
command.

Ref.:
https://spec.oneapi.io/level-zero/latest/core/EXT_Exp_MutableCommandList.html#mutable-command-list

</details>

<details>
<summary>Model driver caching (from v1.5.0)</summary>

Starting from v1.5.0 release the driver allows to cache compiled model in
filesystem. Whenever user compiles a model (using zeGraphCreate), the driver
saves the compiled model representation in the user directory
`~/.cache/ze_intel_npu_cache/` or in the directory specified in
`ZE_INTEL_NPU_CACHE_DIR` environment variable. The cache directory size is
controlled by the driver. Whenever the cache directory exceeds 1GB the least
used compiled models are removed to save the filesystem space.

|Environment variable|Description|
|---|---|
|ZE_INTEL_NPU_CACHE_DIR=<path>|The cache path. Set it to empty ("") to disable driver cache|
|ZE_INTEL_NPU_CACHE_SIZE=<unsigned>|The cache directory size. Whenever the cached files exceeds the size, some cached files are removed using the least recently used strategy|

</details>

<details>
<summary>Kernel module functional tests - npu-kmd-test (from v1.5.0)</summary>

Introduced `npu-kmd-test` in v1.5.0 release that can be used for `intel_vpu`
module validation. `npu-kmd-test` is built together with the driver and can be
found in `<build-dir>/bin`

</details>

<details>
<summary>Support for Arrow Lake and Lunar Lake CPUs (from v1.5.0)</summary>

In v1.5.0 release introduced support for Arrow Lake and Lunar Lake family processors.

</details>

<details>
<summary>Log mask for granular control over logging (from v1.5.0)</summary>

In v1.5.0 release there is new environment variable `ZE_INTEL_NPU_LOGMASK` that
allows the user to enable specific log groups in driver. The
`ZE_INTEL_NPU_LOGLEVEL` variable is used to enable logging. The available log
level has been reduced to `ERROR`, `WARNING` and `INFO`. Whenever `INFO` is
set, the `ZE_INTEL_NPU_LOGMASK` allows to target specific log group. The log
group are listed in
[umd/vpu_driver/source/utilities/log.hpp](../umd/vpu_driver/source/utilities/log.hpp#L19)

```bash
# Set log level to INFO
export ZE_INTEL_NPU_LOGLEVEL=INFO

# Set log mask to only print from DEVICE, DRIVER and CACHE groups
export ZE_INTEL_NPU_LOGMASK=$((1<<4|1<<3|1<<17))
```

</details>

## Supported devices

The Intel® NPU driver supports following Intel® processor families:

* [Meteor Lake](https://ark.intel.com/content/www/us/en/ark/products/codename/90353/products-formerly-meteor-lake.html)
* [Arrow Lake](https://www.intel.com/content/www/us/en/ark/products/codename/225837/products-formerly-arrow-lake.html)
* [Lunar Lake](https://www.intel.com/content/www/us/en/ark/products/codename/213792/products-formerly-lunar-lake.html)

For detailed information on kernel support and NPU driver releases, please refer to the [release
page](https://github.com/intel/linux-npu-driver/releases).

## Kernel module driver

The kernel module driver `intel_vpu` can be found in [drivers/accel
tree](https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git/tree/drivers/accel/ivpu?h=v6.8.1)
in the kernel sources.

<details>
<summary>Enable the intel_vpu module in the kernel config for compilation</summary>

In the Kernel Menu Configuration from `Device Driver` select `Compute Acceleration Framework`
and set "modularize" for `Intel NPU (Neural Processing Unit)`.
</details>
<details>
<summary>Finding the intel_vpu kernel module in the system</summary>

```bash
# check if the intel_vpu exists is in the system
modinfo intel_vpu

# check if the intel_vpu is loaded in the kernel
lsmod | grep intel_vpu

# if the previous command nothing produced, load the intel_vpu
sudo modprobe intel_vpu

# verify that the intel_vpu has been loaded successfully
sudo dmesg | tail -n 20

# check if accel device is available
ls /dev/accel/accel0
```
</details>

## Driver package installation

The driver binary package and installation process can be found in the [release
page](https://github.com/intel/linux-npu-driver/releases). The list of disbributed packages:
* intel-fw-npu: firmware binaries
* intel-level-zero-npu: user space driver library with name libze_intel_npu.so
* intel-driver-compiler-npu: NPU compiler library with name libnpu_driver_compiler.so

## Building a standalone driver

Install the required dependencies in Ubuntu:
```bash
sudo apt update
sudo apt install -y build-essential git git-lfs cmake python3
```

Commands to build the driver:
```bash
cd linux-npu-driver
git submodule update --init --recursive

cmake -B build -S .
cmake --build build --parallel $(nproc)

# install the driver in the system
sudo cmake --install build

# reload the intel_vpu module to load new firmware
sudo rmmod intel_vpu
sudo modprobe intel_vpu
```

## Building a driver together with the compiler

The driver has a target in `compiler/npu_compiler.cmake` to build
Compiler-in-Driver component from [NPU compiler repository](https://github.com/openvinotoolkit/npu_compiler/tree/develop/src/vpux_driver_compiler).

OpenVINO runtime is required by compiler. About the dependencies for building OpenVINO,
please check the [OpenVINO build document](https://github.com/openvinotoolkit/openvino/blob/master/docs/dev/build.md).

To build a compiler the `ENABLE_NPU_COMPILER_BUILD` flag has to be set:
```bash
cd linux-npu-driver
git submodule update --init --recursive

cmake -B build -S . -DENABLE_NPU_COMPILER_BUILD=ON
cmake --build build --parallel $(nproc)

# install the driver in the system
sudo cmake --install build

# reload the intel_vpu module to load new firmware
sudo rmmod intel_vpu
sudo modprobe intel_vpu
```

The compiler binary `libnpu_driver_compiler.so` can be found in `build/lib/`.

> [!Note]
> During compilation, you might encounter the following error on some toolchains or distributions:
> ```bash
> c++: error: unrecognized command line option '-fcf-protection=full'; did you mean '-fstack-protector-all'?
> ```
> Reconfigure the project disabling this flag manually:
> ```bash
> cmake -B build -S . -DENABLE_NPU_COMPILER_BUILD=ON -DCMAKE_CXX_FLAGS="-fcf-protection=none" -DCMAKE_C_FLAGS="-fcf-protection=none"
> ```
> 
> In case non standard compiler build (different then Release config), please follow the guideline
> in [npu_compiler/src/vpux_driver_compiler/README.md](https://github.com/openvinotoolkit/npu_compiler/blob/develop/src/vpux_driver_compiler/README.md#how-to-build-related-targets-locally)

## Driver test application

The `validation/umd-test` directory contains `npu-umd-test` functional tests for driver.
The binary `npu-umd-test` is located in the build folder, ex. `build/bin/`.

Command line to run functional tests (after driver installation):

```bash
npu-umd-test
```

It is expected that `Umd.ConfigurationCheck` test fails when `--config` option
is not used. `--config` points to YAML configuration file that allows to
control the inference test content. Those tests require compiler in system.

Config file requires to download any OpenVINO model. Command line to setup a
`basic.yaml`:
```bash
# Prepare the add_abc model in path pointed by basic.yaml
mkdir -p models/add_abc
curl -o models/add_abc/add_abc.xml https://raw.githubusercontent.com/openvinotoolkit/openvino/master/src/core/tests/models/ir/add_abc.xml
touch models/add_abc/add_abc.bin

# Run tests with add_abc.xml
npu-umd-test --config=validation/umd-test/configs/basic.yaml
```

More information about config can be found in [validation/umd-test/configs](/validation/umd-test/configs).

## Troubleshooting

<details>
<summary>Device is not detectable</summary>

To check if device is available the user can use `npu-umd-test` or `hello_query_device` from the OpenVINO sample applications.
To debug missing NPU device, the `strace` allows to trace system calls that initalize the device. Run test command with `strace`:

```bash
# Record system calls using strace and npu-umd-test
strace -o strace.log --trace=file ./build/bin/npu-umd-test
...
# Or using OpenVINO python API
strace -o strace.log --trace=file python -c "from openvino import Core; print(Core().available_devices)"
...
```
> [!WARNING]
> After v1.16.0 release the driver library has a libze_intel_npu.so.1 name. If you are using
> libze_intel_vpu.so.1 by mistake, please remove it from system

Analyze the `strace.log` file for system calls that open NPU libraries and device:

```bash
grep -E "(accel|libnpu_|libze_)" strace.log
# Below output from command
...
# Check if the Level Zero loader is found in system
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libze_loader.so.1", O_RDONLY|O_CLOEXEC) = 3
....
# libze_intel_vpu.so.1 should not be used after v1.16.0 release, consider to remove it if it is in the system
openat(AT_FDCWD, "/usr/lib/x86_64-linux-gnu/libze_intel_vpu.so.1", O_RDONLY|O_CLOEXEC) = -1 ENOENT (No such file or directory)
...
# Check if driver library is found
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libze_intel_npu.so.1", O_RDONLY|O_CLOEXEC) = 3
...
# Check if driver successfully opened accel0. If unsuccessful, check next section
openat(AT_FDCWD, "/dev/accel/accel0", O_RDWR|O_NOFOLLOW|O_CLOEXEC) = 3
...
# Check if compiler was found
openat(AT_FDCWD, "/lib/x86_64-linux-gnu/libnpu_driver_compiler.so", O_RDONLY|O_CLOEXEC) = 3
...
```
</details>

<details>
<summary>Non-root access to the NPU device</summary>

To access the NPU device, the user must be in the "render" or "video" group.
A group depends on system configuration:
```bash
# check user groups
groups

# add user to the "render" group
sudo usermod -a -G render <user-name>

# log out and log in to apply the new group
```

The patch for systemd to set the "render" group for accel subsystem has been merged,
but might not be available in your Linux distribution. See
[systemd change](https://github.com/systemd/systemd/pull/27785).

If setting the "render" group does not resolve the non-root access issue,
this must be done by an administrator manually:
```bash
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

The compilation may fail due to memory shortage. The recommendation is to use the Ninja generator
instead of Unix Makefiles and extending swap memory. If it does not help, please
[file a new issue](https://github.com/intel/linux-npu-driver/issues/new).

</details>

<details>
<summary>Enable driver log using an environment variable</summary>

Valid logging levels are `ERROR`, `WARNING`, `INFO` (and `VERBOSE` for driver
older than v1.5.0 release)

Seting the logging level using the `ZE_INTEL_NPU_LOGLEVEL` environment variable:
```bash
export ZE_INTEL_NPU_LOGLEVEL=<logging_level>
```

Command to clear an exported value:
```bash
unset ZE_INTEL_NPU_LOGLEVEL
```

Setting `ZE_INTEL_NPU_LOGMASK` allows to print specific log groups in driver.
The log group are listed in
[umd/vpu_driver/source/utilities/log.hpp](../umd/vpu_driver/source/utilities/log.hpp#L19)

```bash
# Set log level to INFO to enable LOGMASK
export ZE_INTEL_NPU_LOGLEVEL=INFO

# Set log mask to only print from DEVICE, DRIVER and CACHE groups
export ZE_INTEL_NPU_LOGMASK=$((1<<4|1<<3|1<<17))
```

</details>

<details>
<summary>Firmware version mismatch</summary>

The user can use different kernel and firmware combination for NPU device. The
user might receive the following error message:

```bash
ERROR! MAPPED_INFERENCE_VERSION is NOT compatible with the ELF Expected: 6.1.0 vs received: 7.0.0
```

It means that NPU compiler mismatches the NPU firmware. To fix this issue the
user needs to upgrade the firmware. Firmware update should be done from
driver repository using release tag that matches the NPU compiler:

```bash
cmake -B build -S .
cmake --install build/ --component fw-npu --prefix /
```
</details>
