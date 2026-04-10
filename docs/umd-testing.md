<!---

Copyright (C) 2025 Intel Corporation

SPDX-License-Identifier: MIT

-->

## Overview

This document describes how to test the NPU driver using the `npu-umd-test` tool.

## Prerequisites

Install the driver and compile the `npu-umd-test` tool as described in the [Installation
Guide](overview.md#driver-package-installation).

Check if the driver is loaded and the device is visible in the system:

```bash
# User should be in the 'render' group to access the device
groups
...

# Check if the driver is loaded
ls /dev/accel/
```

## Prepare npu-umd-test

The `npu-umd-test` tool is not included in the release packages and must be built separately.
The build process is described in the [Installation Guide](overview.md#building-a-standalone-driver).
If the driver is already installed, you can build only the `npu-umd-test` target:

```bash
# Download a repository
git clone https://github.com/intel/linux-npu-driver.git
cd linux-npu-driver/
git submodule update --init --recursive

# Build the npu-umd-test
cmake -B build -S .
cmake --build build/ -j11 --target npu-umd-test

# Run the test to verify setup
./build/bin/npu-umd-test --gtest_filter=Device.GetProperties*

# Make a symbolic link for easy access
cd ../
ln -rsf ./linux-npu-driver/build/bin/npu-umd-test npu-umd-test
```

## Prepare the OpenVINO IR model set

The NPU driver supports models in the OpenVINO IR format. OpenVINO enables conversion of models from various frameworks into this format.
For more details, see the [Conventional Model Preparation](https://docs.openvino.ai/2025/openvino-workflow/model-preparation.html) documentation.

The `npu-umd-test` framework supports running tests using a set of models.
These models must be converted to the OpenVINO IR format prior to execution.

Adding `add_abc.xml` network from [overview.md](overview.md):
```bash
# Prepare the add_abc model in the path indicated in the basic.yaml file
mkdir -p models/add_abc
curl -o models/add_abc/add_abc.xml https://raw.githubusercontent.com/openvinotoolkit/openvino/master/src/core/tests/models/ir/add_abc.xml
touch models/add_abc/add_abc.bin
```

The `mul_add` network has been added to the repository. Below is the instruction on how to download the model from
`linux-npu-driver/validation/models`:
```bash
# When adding another model, remember about the path indicated in the basic.yaml file
mkdir -p models/mul_add
curl -o models/mul_add/mul_add.xml https://raw.githubusercontent.com/intel/linux-npu-driver/main/validation/models/mul_add/mul_add.xml
touch models/mul_add/mul_add.bin
```

### Imagenet Classification Model - ResNet-50

The `npu-umd-test` tool can be used to validate the output of image classification models trained on the ImageNet dataset.
Below is an example showing how to download and convert a ResNet-50 model from PyTorch to the OpenVINO IR format.

```bash
python3 -m venv openvino-venv
source openvino-venv/bin/activate
pip install --upgrade pip
# Pytorch CPU index is added to reduce download time, feel free to remove it
pip install --extra-index-url=https://download.pytorch.org/whl/cpu openvino torch torchvision opencv-python
```

Select a ResNet-50 model from PyTorch and apply image prepostprocessing using OpenVINO. This can be done using the following Python code:

```python
# Download and convert a ResNet-50 to the OpenVINO IR format using Python
import openvino
import os
import torch
import torchvision

model = torchvision.models.resnet50(weights='DEFAULT')
ov_model = openvino.convert_model(model, example_input=torch.rand(1, 3, 224, 224))

ppp = openvino.preprocess.PrePostProcessor(ov_model)
ppp.input().tensor() \
    .set_shape([1,224,224,3]) \
    .set_element_type(openvino.Type.u8) \
    .set_layout(openvino.Layout('NHWC'))
ppp.input().preprocess() \
    .convert_element_type(openvino.Type.f32) \
    .mean([103.94, 116.78, 123.68]) \
    .scale([57.21, 57.45, 57.73])
ppp.input().model().set_layout(openvino.Layout('NCHW'))
ov_model = ppp.build()
openvino.save_model(ov_model, "models/resnet50.xml")
```

Prepare an image by downloading it and resizing it to 224×224 using OpenCV:

```python
# Prepare an image using OpenCV
import cv2
import os
import subprocess

os.makedirs("images", exist_ok=True)
subprocess.run("wget -O images/dog.jpg https://github.com/pytorch/hub/raw/master/images/dog.jpg", shell=True)
image = cv2.imread("images/dog.jpg")
resized_image = cv2.resize(image, (224,224))
cv2.imwrite("images/dog.bmp", resized_image)
```

Check the model prediction:

```python
# Check the model prediction using OpenVINO
import openvino
import numpy as np
import cv2

image = cv2.imread("images/dog.bmp")
tensor = np.expand_dims(image, 0)

compiled_model = openvino.compile_model("models/resnet50.xml", "NPU")
request = compiled_model.create_infer_request()
request.infer(tensor)
predictions = request.get_output_tensor().data
probs = predictions.reshape(-1)
top_10 = np.argsort(probs)[-10:][::-1]

header = 'class_id probability'

print('Top 10 results: ')
print(header)
print('-' * len(header))

for class_id in top_10:
    probability_indent = ' ' * (len('class_id') - len(str(class_id)) + 1)
    print(f'{class_id}{probability_indent}{probs[class_id]:.7f}')
```

Add the model and image to a new `npu-umd-test` configuration file:

```bash
cat <<EOF > resnet_config.yaml

model_dir: models/
image_dir: images/

image_classification_imagenet:
  - path: resnet50.xml
    name: resnet50
    in: [ dog.bmp ]
    class_index: [ 258 ]
EOF
```

Run the image classification model test:

```bash
./npu-umd-test --config=resnet_config.yaml --verbose */resnet50
```

### Object Detection Model – YOLO

The `npu-umd-test` framework does not support output validation for object detection models. 
However, it can still be used to run inference on these models, without performing accuracy verification.

In this section, we will download a YOLOv8 object detection model and convert it to the OpenVINO IR format for further testing.

First, install the Ultralytics package:

```bash
# Install Ultralytics package in same virtual environment
source openvino-venv/bin/activate
pip install ultralytics
```

Download the YOLOv8s model and convert it to the OpenVINO IR format:

```python
# Convert YOLOv8s to the OpenVINO IR format using Python
from ultralytics import YOLO
import os

model = YOLO('yolov8s.pt')
model.export(format="openvino")

os.rename("yolov8s_openvino_model/yolov8s.xml", "models/yolov8s.xml")
os.rename("yolov8s_openvino_model/yolov8s.bin", "models/yolov8s.bin")
```

Add the object detection model to config file:

```bash
cat <<EOF > yolo_config.yaml

model_dir: models/
image_dir: images/

graph_execution:
  - path: yolov8s.xml
    name: yolov8s
    # The GraphQueryNetwork* requires to pass any compiler acceptable flag.
    # TODO: Fix in the next release after v1.23.0
    flags: "--config"
EOF
```

Test the object detection model:

```bash
./npu-umd-test --config=yolo_config.yaml --verbose */yolov8s
```

For more information on Ultralytics and OpenVINO integration, please visit
https://docs.ultralytics.com/integrations/openvino/

### Run tests

The `npu-umd-test` configuration file contains multiple sections,
each described in detail in the [configuration documentation](../validation/umd-test/configs/README.md).

In this example, we will create a configuration file using the models prepared in the previous section.
```yaml
# filename: extend.yaml
model_dir: models/
image_dir: images/

graph_execution:
  - path: add_abc/add_abc.xml
  - path: resnet50.xml
    name: resnet50
    in: [ dog.bmp ]
    class_index: [ 258 ]
  - path: yolov8s.xml
    name: yolov8s

image_classification_imagenet:
  - path: resnet50.xml
    name: resnet50
    in: [ dog.bmp ]
    class_index: [ 258 ]

driver_cache:
  - path: add_abc/add_abc.xml
  - path: resnet50.xml
  - path: yolov8s.xml

multi_inference:
  - name: "ObjectRecognitionPipeline"
    pipeline:
    - path: resnet50.xml
      target_fps: 60
      exec_time_in_secs: 10
    - path: yolov8s.xml
      target_fps: 30
      exec_time_in_secs: 10
```

Run the tests using the newly created configuration file:

```bash
./npu-umd-test --config=extend.yaml
```

### Run additional tests using options

The `npu-umd-test` tool provides additional test cases:

* Driver initialization tests, which must be executed in a separate process:
```bash
./npu-umd-test --ze-init-tests
```
* GPU and NPU tests using the Level Zero API. These require the [compute-runtime](https://github.com/intel/compute-runtime/releases) package to be installed:
```bash
./npu-umd-test --gpu
```
* External memory tests using the System DMA Heap. These require access to /dev/dma_heap/system, which is restricted to root privileges on Ubuntu:
```bash
sudo ./npu-umd-test --dma-heap
```

### Reference test results

UMD test results are published on the [release page](https://github.com/intel/linux-npu-driver/releases/latest).
