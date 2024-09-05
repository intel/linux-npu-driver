<!-- Copyright 2022-2024 Intel Corporation.

 This software and the related documents are Intel copyrighted materials, and
 your use of them is governed by the express license under which they were
 provided to you ("License"). Unless the License provides otherwise, you may
 not use, modify, copy, publish, distribute, disclose or transmit this
 software or the related documents without Intel's prior written permission.

 This software and the related documents are provided as is, with no express
 or implied warranties, other than those that are expressly stated in
 the License. -->

# NPU UMD test configuration overview

The configuration uses the YAML format.
The main purpose of a configuration file is to ensure proper configuration
for a specific platform and allows the user to add own models.
Some test cases are configuration dependent and may or may not exist
depending on the contents of the configuration file.
It is possible to run npu-umd-test without a configuration file, but
only basic generic test cases will be executed and the Umd.ConfigurationCheck
test will fail. Umd.ConfigurationCheck prevents the user from accidentally running
tests without configuration.

---

# Configuration file structure
Each section is optional. The configuration file should consist of at least one valid section.
An empty configuration causes the Umd.ConfigurationCheck test to fail.

The order in which the network is defined is important, API tests only take the first node
from the section.

## Global variables
Global variables define the directories where models, blobs, and images used in tests are stored.
This section also defines the logging level, accepted values ​​are: QUIET, ERROR, WARNING, INFO, VERBOSE.

Example:
```
log_level: ERROR
model_dir: /opt/user/models/
blob_dir: /opt/user/blobs/
image_dir: /opt/user/sample-images/
```

> [!NOTE]
> By default, the model_dir, blob_dir will be added to field **path** in the YAML section.
> The same applies to the image_dir prefix - it will be added to the **in** field containing the image.

## Section graph\_execution
Defines a list of models used in most graph tests.

This section consists of fields:
- **path:** path to the XML file or compiled model in native binary format represented by the blob
- **name:** test name, this name will be displayed when the test is executed
- **flags:** compilation flags passed directly to the compiler
- **in:** image or binary data
- **out:** expected output
- **class_index:** expected image class index
- **graph_profiling:** bool, if defined and set to false, graph profiling tests will be disabled

Example:
```
graph_execution:
  - path: mobilenet-v2/vpuip.blob
    name: mobilenet-v2
    in: [ input-0.bin ]
    out: [ exp-output-0.bin ]
    graph_profiling: false
  - path: public/resnet-50-pytorch.xml
    name: resnet-50-pytorch_FP16-INT8_AS_NHWC_NC_U8_FP32
    flags: --inputs_precisions="result.1:u8" --inputs_layouts="result.1:NHWC" --outputs_precisions="495:fp32" --outputs_layouts="495:NC"
    in: [ husky.bmp ]
    class_index: [ 248 ]
```

## Section graph\_metrics
Defines a list of models used in metric tests.

This section consists of fields:
- **path:** path to the XML file
- **name:** test name, this name will be displayed when the test is executed
- **flags:** compilation flags passed directly to the compiler
- **metric_groups:** the name of the metric group that will be used for testing
- **inference_concurrency:** indicates how many requests will be executed simultaneously

Example:
```
graph_metrics:
  - path: public/mobilenet-v2.xml
    name: mobilenet-v2_FP16-INT8_AS_NCHW_NC_FP32_FP32_THROUGHPUT
    flags: --inputs_precisions="result.1:FP32" --inputs_layouts="result.1:NCHW" --outputs_precisions="473:FP32" --outputs_layouts="473:NC" --config PERFORMANCE_HINT="THROUGHPUT"
    metric_groups: [ NOC ]
    inference_concurrency: 4
```

## Section image\_classification\_imagenet
This section defines the models used in accuracy tests.

It should contain:
- **path:** path to XML file
- **name:** test name, this name will be displayed when the test is executed
- **flags:** compilation flags passed directly to the compiler
- **in:** image used as input to the network
- **class_index:** expected image class index

Example:
```
image_classification_imagenet:
  - path: public/resnet-50-pytorch.xml
    name: resnet-50-pytorch_FP16-INT8_AS_NHWC_NC_U8_FP32
    flags: --inputs_precisions="result.1:u8" --inputs_layouts="result.1:NHWC" --outputs_precisions="495:fp32" --outputs_layouts="495:NC"
    in: [ cat3.bmp ]
    class_index: [ 284 ]
```

## Section multi\_inference
This section is mainly used in the CompilerInDriverMultiInference.Pipeline test.
All defined models are compiled and then executed simultaneously in separate threads
with a target frame rate. The input and class index are optional. When **in** is not defined,
the input will be filled with random values.

A section may consist of fields:
- **path:** path to XML file
- **flags:** compilation flags passed directly to the compiler
- **in:** optional, image used as an input for network
- **class_index:** optional, expected image class index
- **target\_fps:** target fps rate
- **exec\_time\_in\_secs:** execution time in seconds
- **priority:** command queue priority, available priority levels: high, low, normal
- **delay_in_us:** wait a specified period of time before starting the inference

Example:
```
multi_inference:
  - name: "ImageClassificationNetwork"
    pipeline:
    - path: public/resnet-50-pytorch.xml
      flags: --inputs_precisions="result.1:u8" --inputs_layouts="result.1:NHWC" --outputs_precisions="495:fp32" --outputs_layouts="495:NC"
      in: [ cat3.bmp ]
      class_index: [ 284 ]
      target_fps: 30
      exec_time_in_secs: 10
    - path: public/resnet-50-pytorch.xml
      flags: --inputs_precisions="result.1:u8" --inputs_layouts="result.1:NHWC" --outputs_precisions="495:fp32" --outputs_layouts="495:NC"
      in: [ watch.bmp ]
      class_index: [ 826 ]
      target_fps: 30
      exec_time_in_secs: 10
    - path: public/mobilenet-v2.xml
      flags: --inputs_precisions="result.1:u8" --inputs_layouts="result.1:NHWC" --outputs_precisions="473:fp32" --outputs_layouts="473:NC"
      target_fps: 30
      exec_time_in_secs: 10
```

---

# Running tests with a configuration file

There are two options for specifying a configuration file: `-c` or `--config`
```
./npu-umd-test --config=<full_path>/config.yaml
```
or
```
./npu-umd-test -c <full_path>/config.yaml
```

When the configuration file is placed in the default location, it is not necessary to provide the full path:
```
./npu-umd-test --config=config.yaml
```

The config.yaml will be searched in the following locations:
```
./
/usr/share/vpu/
/usr/share/vpu/validation/umd-test/configs/
```
