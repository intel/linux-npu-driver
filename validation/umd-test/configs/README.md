<!-- Copyright 2022-2024 Intel Corporation.

 This software and the related documents are Intel copyrighted materials, and
 your use of them is governed by the express license under which they were
 provided to you ("License"). Unless the License provides otherwise, you may
 not use, modify, copy, publish, distribute, disclose or transmit this
 software or the related documents without Intel's prior written permission.

 This software and the related documents are provided as is, with no express
 or implied warranties, other than those that are expressly stated in
 the License. -->

# VPU Umd test configuration overview

The configuration uses yaml format.
The main purpose of configuration file is to provide right configuration
for particular platform and driver release and it allows user to add
own models and blobs.
Some tests cases depends on configuration and they can be present or not
depending on the contents of the configuration file.
It is possible to run vpu-umd-test without configuration file, in this case
only basic generic testcases would be executed and Umd.ConfigurationCheck test
fails. Umd.ConfigurationCheck prevent user to incidentally run tests without configuration.

---

# Configuration file structure
Each section is optional. Configuration file should consist at least one valid section.
Empty configuration causes that Umd.ConfigurationCheck test fails.

## Global variables
Defines directories where models, blobs and pictures are stored that are used by tests

In this section is also defined logging level, accepted values are: QUIET, ERROR, WARNING, INFO, VERBOSE

Example:
```
log_level: ERROR
model_dir: /opt/user/models/
blob_dir: /opt/user/blobs/
image_dir: /opt/user/sample-images/

```

## Section "graph\_execution"
Defines list of compiled blobs used for graph execution tests from groups:
"CommandGraph\*.\*, GraphInference.\*, GraphNative\*.\*, InferencePerformance.\*"

Order of defining blobs is significant, simple tests takes only first blob from this
section most complex executes all defined.

There must be specified:
- **path:** path to blob binary, "blob_dir" prefix will added to this by default
- **name:** test name, this name will be displayed when test will be executed
- **in:** input data, can be specified more than one
- **out:** expected output, can be specified more than one

Example:
```
graph_execution:
   - path: mobilenet-v2/vpuip.blob
     name: mobilenet-v2
     in: [ input-0.bin ]
     out: [ exp-output-0.bin ]
   - path: yolo-v4-tiny/vpuip.blob
     name: yolo-v4-tiny
     in: [ input-0.bin ]
     out: [ exp-output-0.bin, exp-output-1.bin ]

```

## Section "graph\_metrics"
Defines list of blobs used by metric and metric streamer tests:
"MetricStreamer.\* , MetricQuery.\*"

Section definition is similar to "graph\_execution"(see above) there is one additional flag:
- **act\_shave\_tasks:** true means that blob has activation shave tasks and all counters are validated

Example:

```
graph_metrics:
   - path: mobilenet-v2/vpuip.blob
     name: mobilenet-v2
     in: [ input-0.bin ]
     out: [ exp-output-0.bin ]
     metric_groups: [ NOC ]
```


## Section "compiler\_in\_driver"
Defines list of models used to test compiler in driver.
It is used by tests:
"CompilerInDriver.\*, CompilerInDriverLayers.\*, CompilerInDriverLong.\*, CompilerInDriverWithProfiling.\*"
There must be specified:
- **path:** path to model to compile, the generated test name will be the name of model
- **flags:** compilation flags passed directly to compiler
- **graph_profiling:** if the flag is set to "false", graph profiling tests are disabled

Example:

```
compiler_in_driver:
  - path: add_abc/add_abc.xml
    flags: --inputs_precisions="A:fp16 B:fp16 C:fp16" --inputs_layouts="A:C B:C C:C" --outputs_precisions="Y:fp16" --outputs_layouts="Y:C"
  - path: public/mobilenet-v2/onnx/FP16-INT8/mobilenet-v2.xml
    flags: --inputs_precisions="result.1:u8" --inputs_layouts="result.1:NHWC" --outputs_precisions="473:fp32" --outputs_layouts="473:NC"
    graph_profiling: false
```

## Section "image\_classification\_imagenet"
This section defines models used in image classificiation tests:
"CompilerInDriverLongBmp.\*"
For each model must be specified:
- **path:** path to model to compile, the generated test name will be the name of model
- **flags:** compilation flags passed directly to compiler
- **input:** images list used as an input for network, "image\_dir" prefix will added to this by default
- **class_index:** expected class index for each image
- **iterations:** number of iterations for each network

Example:
```
image_classification_imagenet:
  - path: public/resnet-50-pytorch/onnx/FP16-INT8/resnet-50-pytorch.xml
    flags: --inputs_precisions="result.1:u8" --inputs_layouts="result.1:NHWC" --outputs_precisions="495:fp32" --outputs_layouts="495:NC"
    input: [ cat3.bmp, watch.bmp ]
    class_index: [ 283, 531 ]
    iterations: 100
```
## Section "multi\_inference"
This configuration is used by single CompilerInDriverMultiInference.Pipeline test
All defined models are compiled and then executed simultanously in separate threads with target fps rate.
The input and class_index are optional, when input is not defined the random data is passed to network

For each model can be specified:
- **path:** path to model to compile, the generated test name will be the name of model
- **flags:** compilation flags passed directly to compiler
- **input:** optional, images list used as an input for network, "image\_dir" prefix will added to this by default
- **class_index:** optional, expected class index for each image
- **target\_fps:** target fps rate
- **exec\_time\_in\_secs:** execution time in seconds
- **priority:** set command queue priority, available priority levels: high, low, normal
- **delay_in_us:** wait for specific time before starting the inference

Example:
```
multi_inference:
  - name: "ImageClassificationNetworks"
    pipeline:
    - path: public/resnet-50-pytorch/onnx/FP16-INT8/resnet-50-pytorch.xml
      flags: --inputs_precisions="result.1:u8" --inputs_layouts="result.1:NHWC" --outputs_precisions="495:fp32" --outputs_layouts="495:NC"
      input: [ watch.bmp ]
      class_index: [ 531 ]
      target_fps: 30
      exec_time_in_secs: 10
    - path: public/mobilenet-v2/onnx/FP16-INT8/mobilenet-v2.xml
      flags: --inputs_precisions="result.1:u8" --inputs_layouts="result.1:NHWC" --outputs_precisions="473:fp32" --outputs_layouts="473:NC"
      target_fps: 30
      exec_time_in_secs: 10
```

---

# Running tests with configuration
There are two options to point configuration file: -c or --config

./vpu-umd-test --config=/fullpath/config.yaml

or

./vpu-umd-test -c /fullpath/config.yaml

vpu-umd-test application has hardcoded some default paths to lockup for configuration,
used when full path is not provided.
When configuration file is placed in default location it is not necessary to specify full path.

For example:

./vpu-umd-test --config=config.yaml

The configuration file config.yaml will be searched in the following locations:
- ./
- /usr/local/share/vpu/
- /usr/local/share/vpu/validation/umd-test/configs/

