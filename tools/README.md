<!---

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

-->

# intel-npu-smi

`intel-npu-smi` is a system monitoring tool for the Intel NPU. It reads NPU telemetry exposed
through the Intel PMT (Platform Monitoring Technology) sysfs interface and reports it in a
human-readable form, either as a single snapshot or as a continuously refreshing live view.

Source: `tools/intel-npu-smi/`

## What it reports

- **Power** - accumulated NPU energy consumption.
- **Frequency and voltage** - current NPU operating workpoint.
- **Temperature** - SoC temperature reading that includes the NPU die.
- **Memory bandwidth** - accumulated NPU DDR/NoC traffic.
- **Active processes** - PID, command line and memory usage (KiB) of processes currently using the NPU.

## Example output

A single snapshot (`intel-npu-smi`, no flags) prints a boxed dashboard with device info, current
power/frequency/bandwidth/tile config, temperature/utilization/memory, and (if any) a table of
active NPU processes. Real output captured on a Panther Lake system:

```
+-----------------------------------------------------------------------------------------------+
| INTEL NPU Device: 0xb03e | Driver version:                                              1.0.0 |
| Firmware version: Aug 20 2026*NPU50xx*build/ci/npu-fw-ci-ci_branch_UD202638_npu_release_26ww3 |
| 2-20260813_185051-3a5e4dd1cf93522baa79e55a53f2ac7165723cad-1-g6fc835a1920*6fc835a192052ac2261c4e1d5da7379ced95ae81|
+===============================================================================================+
|       Power Usage        |     DPU Frequency    | NPU DDR Average Bandwidth |   Tile Config   |
|                 0.00 [W] |               0 [Hz] |             0.000 [MiB/s] |               0 |
+===============================================================================================+
|     NPU Temperature      |       NPU Utilization       |                          Memory Usage|
|                  22 [°C] |                       0 [%] |                          65.53 [MiB] |
+===============================================================================================+
```

All metrics are `0` here because the NPU was idle at the time of the snapshot - no inference
workload was running. The process table at the bottom is only shown when at least one process is
using the NPU.

## Field units

| Field                     | Unit                 |
|---------------------------|----------------------|
| Power Usage               | Watts (W)            |
| DPU Frequency             | Hertz (Hz)           |
| NPU DDR Average Bandwidth | MiB/s or GiB/s       |
| Tile Config               | tile count (integer) |
| NPU Temperature           | Celsius (°C)         |
| NPU Utilization           | Percent (0-100%)     |
| Memory Usage              | KiB, MiB or GiB      |
| Process Memory            | KiB, MiB or GiB      |

Power and bandwidth are not instantaneous readings - they are both computed as the delta between
two telemetry samples divided by the time between them (energy difference for power, raw counter
difference for bandwidth). Even a single snapshot (no `-i`) takes two samples internally, 200ms
apart, so these fields reflect real NPU activity during that short window - they read `0` simply
when the NPU was idle at the time, not because of a missing second sample.

## Limitations

- Only a single NPU device is reported. Device discovery stops at the first PCI device with a bound
  `accel` interface under `/sys/bus/pci/drivers/intel_vpu/` - on a system with multiple NPUs, only the
  first one found is shown.

## Troubleshooting

| Message                         | Cause / fix                                                          |
|----------------------------------|-----------------------------------------------------------------------|
| `... is not loaded.`            | Load the kernel module: `modprobe intel_vpu`.                        |
| `No NPU device found.`          | `intel_vpu` loaded but no bound `accel` device; check `lspci`/dmesg. |
| Permission denied reading sysfs | Run with `sudo` - the tool reads driver sysfs/debugfs files.         |

## Supported platforms

- Meteor Lake (MTL)
- Arrow Lake (ARL)
- Lunar Lake (LNL)
- Panther Lake (PTL)
- Wildcat Lake (WCL)

Each platform exposes its PMT telemetry registers at different byte offsets within the telemetry buffer:

| Register           | MTL / ARL | LNL    | PTL / WCL |
|---------------------|-----------|--------|-----------|
| `VPU_ENERGY`        | `0x628`   | `0x5d0`| `0x670`   |
| `SOC_TEMPERATURES`  | `0x98`    | `0x70` | `0x78`    |
| `VPU_WORKPOINT`     | `0x68`    | `0x18` | `0x18`    |
| `VPU_MEMORY_BW`     | `0x0`     | `0xc18`| `0xc18`   |

The correct offset map is selected at runtime by matching the PMT telemetry aggregator's GUID
(under `/sys/class/intel_pmt/telem*/guid`) against a set of known per-platform values.

## Building

`intel-npu-smi` is a regular CMake target (`tools/CMakeLists.txt` adds the `tools/intel-npu-smi`
subdirectory) in the repository. Building it requires `ENABLE_TOOLS_BUILD=ON`, since it's disabled
by default.

```bash
cmake -S . -B build -DENABLE_TOOLS_BUILD=ON -DENABLE_VALIDATION_BUILD=OFF
cmake --build build --target intel-npu-smi -j$(nproc)
```

The resulting binary is at `build/bin/intel-npu-smi`. `ENABLE_VALIDATION_BUILD=OFF` is optional -
it just skips building the (unrelated) validation test suite to speed up the build.

## Running

The tool reads telemetry from `/sys/bus/pci/drivers/intel_vpu/`, so the `intel_vpu` kernel module must be loaded,
and the tool typically needs to be run with elevated privileges to access sysfs.

```bash
# Single snapshot (default)
sudo ./intel-npu-smi

# Continuous monitoring every 500 ms, with colored output
sudo ./intel-npu-smi -i 500 -c

# Continuous monitoring, exporting metrics to a CSV file
sudo ./intel-npu-smi -i 1000 --csv log.csv
```

### Options

| Flag                   | Description                                                                 |
|------------------------|------------------------------------------------------------------------------|
| `-i, --interval <ms>`  | Update interval in milliseconds (minimum 200ms). Without it, a single snapshot is printed. |
| `-c, --color`          | Enable colored terminal output.                                              |
| `-v, --verbose`        | Enable verbose logging.                                                      |
| `--csv <file>`         | Export metrics to a CSV file.                                                |
| `-h, --help`           | Display the help message.                                                    |
| `--version`            | Display version information.                                                 |
