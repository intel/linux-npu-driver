<!-- Copyright 2025-2026 Intel Corporation.

 This software and the related documents are Intel copyrighted materials, and
 your use of them is governed by the express license under which they were
 provided to you ("License"). Unless the License provides otherwise, you may
 not use, modify, copy, publish, distribute, disclose or transmit this
 software or the related documents without Intel's prior written permission.

 This software and the related documents are provided as is, with no express
 or implied warranties, other than those that are expressly stated in
 the License. -->

# Table of Contents

- [Logging NPU UMD events](#logging-npu-umd-events)
  - [Using UMD log masks](#using-umd-log-masks)
- [Logging NPU compiler events](#logging-npu-compiler-events)
- [Offline compilation with null device mode](#offline-compilation-with-null-device-mode)
- [Caching driver models](#caching-driver-models)
- [Tracing with Perfetto](#tracing-with-perfetto)
  - [Recording in-app traces](#recording-in-app-traces)
  - [Recording system traces](#recording-system-traces)
  - [Visualizing traces](#visualizing-traces)
  - [Printing traces to terminal](#printing-traces-to-terminal)

# Logging NPU UMD events

Below are the commands to enable NPU UMD Log in different levels:
```cpp
export ZE_INTEL_NPU_LOGLEVEL=ERROR
export ZE_INTEL_NPU_LOGLEVEL=WARNING
export ZE_INTEL_NPU_LOGLEVEL=INFO
```

## Using UMD log masks

Additionally user can filter UMD logs by `ZE_INTEL_NPU_LOGMASK` environment variable.
```cpp
# ZE_INTEL_NPU_LOGMASK mapping:
CMDLIST = BIT(0),  // 0x1
CMDQUEUE = BIT(1), // 0x2
CONTEXT = BIT(2),  // 0x4
DEVICE = BIT(3),   // 0x8
DRIVER = BIT(4),   // 0x10
EVENT = BIT(5),    // 0x20
FENCE = BIT(6),    // 0x40
FSYS = BIT(7),     // 0x80
GRAPH = BIT(8),    // 0x100
IOCTL = BIT(9),    // 0x200
MEMORY = BIT(10),  // 0x400
METRIC = BIT(11),  // 0x800
MISC = BIT(12),    // 0x1000
UTEST = BIT(13),   // 0x2000
VPU_CMD = BIT(14), // 0x4000
VPU_CTX = BIT(15), // 0x8000
VPU_JOB = BIT(16), // 0x10000
CACHE = BIT(17),   // 0x20000
API = BIT(18),     // 0x40000
API_DDI = BIT(19), // 0x80000
API_EXT = BIT(20), // 0x100000
```
By default `ZE_INTEL_NPU_LOGMASK` is set to enable all upper components (without FSYS, METRIC, UTEST) in UMD logs.

For example, to change ZE_INTEL_NPU_LOGMASK and print only DRIVER, DEVICE, IOCTL UMD logs:
```cpp
export ZE_INTEL_NPU_LOGLEVEL=INFO
export ZE_INTEL_NPU_LOGMASK=$((1<<3 | 1<<4 | 1<<9))
```

To clear export value:
```cpp
unset ZE_INTEL_NPU_LOGLEVEL
unset ZE_INTEL_NPU_LOGMASK
```

# Logging NPU compiler events

Below are the commands to enable NPU Compiler Log in different levels:
```cpp
export ZE_INTEL_NPU_COMPILER_LOGLEVEL=ERROR
export ZE_INTEL_NPU_COMPILER_LOGLEVEL=WARNING
export ZE_INTEL_NPU_COMPILER_LOGLEVEL=INFO
export ZE_INTEL_NPU_COMPILER_LOGLEVEL=DEBUG
export ZE_INTEL_NPU_COMPILER_LOGLEVEL=TRACE

# To clear export value
unset ZE_INTEL_NPU_COMPILER_LOGLEVEL
```

# Offline compilation with null device mode

> [!IMPORTANT]
> To use offline compilation it is required to build driver with the `ENABLE_OFFLINE_COMPILATION_SUPPORT` flag set to ON.

> [!WARNING]
> The driver allows running workloads in "null driver" mode, but the result of execution is undefined. This mode is intended for offline compilation only.

To control offline compilation, the driver uses the following environment variables:

| Environment variable name             | Description                                                                                                                                                       |
| ------------------------------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ZE_INTEL_NPU_PLATFORM_OVERRIDE        | Enables "null driver" mode and sets platform. Supported values (long and short names):                                                                            |
|                                       | - METEORLAKE (INPU_MTL)                                                                                                                                           |
|                                       | - LUNARLAKE (INPU_LNL)                                                                                                                                            |
|                                       | - PANTHERLAKE (INPU_PTL)                                                                                                                                          |
|                                       | - WILDCATLAKE (INPU_WCL)                                                                                                                                          |
|                                       | - ARROWLAKE                                                                                                                                                       |
| ZE_INTEL_NPU_REVISION_OVERRIDE        | Sets the revision number.                                                                                                                                         |
|                                       | Should be set in numeric format. Decimal, octal and hexadecimal numbers are accepted. When not provided the default value -1 is set.                              |
| ZE_INTEL_NPU_TILE_COUNT_OVERRIDE      | Sets the number of tiles that are enabled. This value cannot be set to 0 or to a number greater than the number of tiles on the platform.                         |
|                                       | Acceptable formats are decimal, octal, or hexadecimal. When not set, all tiles are enabled by default.                                                            |
| ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE   | Directly sets the mask of disabled tiles.                                                                                                                         |
|                                       | This variable cannot be used together with `ZE_INTEL_NPU_TILE_COUNT_OVERRIDE`, which overrides provided mask.                                                     |
|                                       | Accepted formats are: binary (example: b010), octal (example: 02), decimal (example: 2), hexadecimal (example: 0x2).                                              |


The driver recommends using only `ZE_INTEL_NPU_PLATFORM_OVERRIDE` and omitting other environment
variables unless necessary. Not setting any other variables allows the compiler to choose optimal
values for the given platform.

Example environment variable settings to compile model for Lunar Lake platform:

```bash
# Set only platform, rest will be chosen by the compiler:
export ZE_INTEL_NPU_PLATFORM_OVERRIDE=LUNARLAKE
# Or use the short name:
# export ZE_INTEL_NPU_PLATFORM_OVERRIDE=INPU_LNL
```

More advanced scenario:
```bash
# NPU revision 4, 5 tiles from 6 are enabled:
export ZE_INTEL_NPU_PLATFORM_OVERRIDE=LUNARLAKE
export ZE_INTEL_NPU_REVISION_OVERRIDE=0x4
export ZE_INTEL_NPU_TILE_COUNT_OVERRIDE=0x5
```

Example of using tile mask:
```bash
# NPU revision 1, 4 tiles from 6 are enabled, a tile mask is used:
export ZE_INTEL_NPU_PLATFORM_OVERRIDE=LUNARLAKE
export ZE_INTEL_NPU_REVISION_OVERRIDE=0x1
export ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE=b101000
```

To clear export values:
```bash
# Reset all environment variables:
unset ZE_INTEL_NPU_PLATFORM_OVERRIDE
unset ZE_INTEL_NPU_REVISION_OVERRIDE
unset ZE_INTEL_NPU_TILE_COUNT_OVERRIDE
unset ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE
```

# Caching driver models

The driver is able to cache compiled blobs in the user home directory `~/.cache/ze_intel_npu_cache/`
or in the directory specified in the `ZE_INTEL_NPU_CACHE_DIR` variable.

First, the driver creates a hash based on the user's ze_graph_desc_t, driver version, and compiler version.
This hash is used to check whether a blob already exists in the cache directory.
If not, after the compilation, the blob will be written to the cache directory with the name set to hash.


| Environment variable name             | Description                                                                                                                                                    |
| :-----------------------------------: | :--------------------------------------------------------------------------------------------------------------------------------------------------------------|
| ZE_INTEL_NPU_CACHE_DIR=\<path\>       | The cache path. To disable the driver cache, set it to empty ("").                                                                                             |
| ZE_INTEL_NPU_CACHE_SIZE=\<unsigned\>  | The size of blobs stored in cache path. Whenever the cached files exceed the size, some cached files are removed using the LRU (least recently used) strategy. |

# Tracing with Perfetto

This section provides the basics of using Perfetto to visualize UMD L0 core API callbacks from user applications
and system calls used by NPU UMD.

> [!IMPORTANT]
> To use Perfetto tracing it is required to build driver with the `ENABLE_NPU_PERFETTO_BUILD` flag set to ON.

## Recording in-app traces

Perfetto allows to record various UMD events from user applications. For more
information see [Recording In-App Traces](https://perfetto.dev/docs/getting-started/in-app-tracing)
in Perfetto documentation

1. To enable Perfetto recording set `ZE_INTEL_NPU_LOGPERFETTO` environment variable:
```cpp
export ZE_INTEL_NPU_LOGPERFETTO=1
```
2. Run user workload. After starting the application, the following messages are visible:
```cpp
[059.440]       perfetto.cc:47922 Configured tracing session 1, #sources:1, duration:0 ms, #buffers:1, total buffer size:65536 KB, total sessions:1, uid:0 session name: ""

...

[069.995]       perfetto.cc:49628 Tracing session 1 ended, total sessions:0
```
3. A `intel-npu-umd.perfetto-trace` file should be created.

## Recording system traces

Perfetto allows to record system calls and other ftrace events together with UMD tracing. See
[Recording system trace](https://perfetto.dev/docs/getting-started/system-tracing) in Perfetto documentation

1. To enable system trace recording set `ZE_INTEL_NPU_LOGPERFETTO=system` environment variable:
```bash
export ZE_INTEL_NPU_LOGPERFETTO="system"
```

2. Download and setup the Perfetto binaries from the [Perfetto releases](https://github.com/google/perfetto/releases/latest):
```bash
wget https://github.com/google/perfetto/releases/download/v51.2/linux-amd64.zip
unzip linux-amd64.zip
chmod +x linux-amd64/*
mv linux-amd64 perfetto
```

3. Setup the config file:
```bash
cat <<EOF > config.pbtx
buffers {
  size_kb: 65536
  fill_policy: DISCARD
}

data_sources {
  config {
    name: "track_event"
    track_event_config {
      enabled_categories: "*"
    }
  }
}

data_sources {
  config {
    name: "linux.process_stats"
    process_stats_config {
      proc_stats_poll_ms: 100
    }
  }
}

data_sources {
  config {
    name: "linux.ftrace"
    ftrace_config {
      ftrace_events: "raw_syscalls/sys_enter"
      ftrace_events: "raw_syscalls/sys_exit"
      ftrace_events: "sched/sched_process_exit"
      ftrace_events: "sched/sched_process_free"
      ftrace_events: "task/task_newtask"
      ftrace_events: "task/task_rename"
    }
  }
}

duration_ms: 60000
EOF
```

4. Start the Perfetto tracing session:
```bash
# Remove leftovers from previous sessions
sudo rm /tmp/perfetto-* -rf

# Start the tracing daemon
./perfetto/traced &
sudo ./perfetto/traced_probes &

# Wait for the tracing daemon to start
sleep 3

# Start the tracing recording
./perfetto/perfetto --txt --config config.pbtx --out intel-npu-umd.perfetto-trace &

# Don't forget to set the environment variable
export ZE_INTEL_NPU_LOGPERFETTO="system"

# Run any workload
npu-umd-test --config=ptl.yaml GraphInference.InferenceTest/*

# Stop the tracing recording
pkill -SIGINT perfetto

# Wait for the tracing to finish gracefully
sleep 1

# Clean up the tracing daemon
sudo pkill traced_probes
pkill traced
```

> [!TIP]
> `traced`, `traced_probes` and `perfetto` can be replaced with `tracebox`. `tracebox` works with
> root. The manual explains how to set up tracing using user with no root privileges.
>

## Visualizing traces
The captured trace can be visually explored by using a dedicated web-based UI.

1. Navigate to [ui.perfetto.dev](https://ui.perfetto.dev/) in a browser.
2. Click the **Open trace file** on the left-hand menu, and load the captured trace.
3. Explore the trace by zooming/panning using WASD. Press "?" for further navigation controls.

> [!TIP]
> For more information visit https://perfetto.dev/docs/.
>

## Printing traces to terminal
To print the trace to terminal, use the `trace_processor_shell` tool from the Perfetto package
mentioned in [Recording system traces](#recording-system-traces). Create a file with a query, for example `query.sql`:
```sql
SELECT
  name,
  COUNT(*) AS call_count,
  AVG(dur) / 1e6 AS avg_duration_ms
FROM
  slice
WHERE
  name LIKE 'zeCommandQueue%'
OR
  name LIKE 'zeCommandList%'
GROUP BY name
```

Then run the query using the `trace_processor_shell` tool:
```bash
$ ./perfetto/trace_processor_shell -q query.sql -W intel-npu-umd.perfetto-trace
[772.661] processor_shell.cc:1889 Trace loaded: 0.47 MB in 0.01s (32.9 MB/s)
column 0 = name
column 1 = call_count
column 2 = avg_duration_ms
"name","call_count","avg_duration_ms"
"zeCommandListClose",1321,0.000942
"zeCommandListCreate",4,0.000946
"zeCommandListDestroy",4,0.109339
"zeCommandListGetNextCommandIdExp",4,0.000445
"zeCommandQueueCreate",1,1.442992
"zeCommandQueueDestroy",1,1.876244
"zeCommandQueueExecuteCommandLists",1321,0.013365
[772.663] _processor_shell.cc:617 Query execution time: 1 ms
```

# Reducing UMD binary size
Users that care about driver binary size, can reduce it with following compile options:
```bash
cmake -S . -B build -DENABLE_NPU_LOGGING=OFF -DENABLE_NPU_PERFETTO_BUILD=OFF
cmake --build build -j
cmake --install build --prefix install --strip
```

Please note that it will reduce driver debugability, meaning no debugging symbols, no Perfetto traces, and no driver logs will be available.
