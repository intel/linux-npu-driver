<!---

Copyright (C) 2025 Intel Corporation

SPDX-License-Identifier: MIT

-->

## Overview

This document provides a description how to test the NPU driver using npu-umd-test tool

## Prerequisites

Install the driver and compile the npu-umd-test tool as described in the [Installation Guide](overview.md).

Check if the driver is loaded and the device is visible in the system:

```bash
# User should be in the 'render' group to access the device
$ groups
...

# Check if the driver is loaded
$ ls /dev/accel/

# Check if the device is visible from the user space driver
# If npu-umd-test is not installed, see next section.
$ npu-umd-test --gtest_filter=Device.*
```

## Build npu-umd-test

The npu-umd-test are built together with the driver. The build process is described in the
[Installation Guide](overview.md). If driver is already installed, you can build the npu-umd-test
tool separately:

```bash
# Prepare a repository
git clone https://github.com/intel/linux-npu-driver.git
cd linux-npu-driver/
git submodule update --init --recursive

# Build the npu-umd-test
cmake -B build -S .
cmake --build build/ -j11 --target npu-umd-test

# Run the test to verify setup
./build/bin/npu-umd-test --gtest_filter=Device.GetProperties*

# Make a symbolic link for easy access
ln -rsf ./build/bin/npu-umd-test npu-umd-test
```

## Prepare model set

The NPU driver supports models in the OpenVINO IR format. OpenVINO allows to convert model from
different frameworks, see [Conventional Model Preparation](https://docs.openvino.ai/2025/openvino-workflow/model-preparation.html) docs.

The npu-umd-test is able to run tests with single model or with a set of models. The models have to be converted to the OpenVINO IR format.

As first model we can add `add_abc.xml` from [overview.md](overview.md)
```bash
# Prepare the add_abc model in path pointed by basic.yaml
mkdir -p models/
curl -o models/add_abc.xml https://raw.githubusercontent.com/openvinotoolkit/openvino/master/src/core/tests/models/ir/add_abc.xml
touch models/add_abc.bin
```

### Example: Imagenet Classification Model

The npu-umd-test allows to test output of image classification model trained on ImageNet dataset. Below is an
example of how to download and convert a ResNet-50 model from Pytorch to OpenVINO IR format.

```bash
python3 -m venv openvino-venv
source openvino-venv/bin/activate
pip install --upgrade pip
# Pytorch CPU index is added to reduce download time, feel free to remove it
pip install --extra-index-url=https://download.pytorch.org/whl/cpu openvino torch torchvision opencv-python
```

Pick up ResNet-50 from PyTorch and apply image prepostprocessing using OpenVINO
```python
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

Prepare an image
```python
import urllib
import cv2

os.makedirs("images", exist_ok=True)
url, filename = ("https://github.com/pytorch/hub/raw/master/images/dog.jpg", "images/dog.jpg")
try: urllib.URLopener().retrieve(url, filename)
except: urllib.request.urlretrieve(url, filename)

image = cv2.imread("images/dog.jpg")
resized_image = cv2.resize(image, (224,224))
cv2.imwrite("images/dog.bmp", resized_image)
```

Check model prediction
```python
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

print('')
```

Add the model and the image to the new npu-umd-test config file:
```bash
$ cat <<EOF > extend.yaml

model_dir: models/
image_dir: images/

image_classification_imagenet:
  - path: resnet50.xml
    name: resnet50
    in: [ dog.bmp ]
    class_index: [ 258 ]
EOF

# Test the change
$ ./npu-umd-test --config=extend.yaml --verbose */resnet50
```

### Example: Object Detection Model - Yolo

The npu-umd-test does not supports output validation for object detection models. The npu-umd-test framework can still be used to run inference on these models. Below is an example of how to download and convert a YOLOv8 object detection model to OpenVINO IR format.

```bash
# Activate the same virtual environment created in the image classification example
source openvino-venv/bin/activate
pip install ultralytics
```

```python
from ultralytics import YOLO
import os

model = YOLO('yolov8s.pt')
model.export(format="openvino")

os.rename("yolov8s_openvino_model/yolov8s.xml", "models/yolov8s.xml")
os.rename("yolov8s_openvino_model/yolov8s.bin", "models/yolov8s.bin")
```

Add the object detection model to your config file:

```bash
$ cat <<EOF > extend.yaml

model_dir: models/
image_dir: images/

graph_execution:
  - path: yolov8s.xml
    name: yolov8s
    # The GraphQueryNetwork* requires to pass any compiler acceptable flag.
    # TODO: Fix in the next release after v1.23.0
    flags: "--config"
EOF

# Test the object detection model
$ ./npu-umd-test --config=extend.yaml --verbose */yolov8s
```

For more information about Ultralytics and OpenVINO integration check https://docs.ultralytics.com/integrations/openvino/

### Run tests

There is multiple section in the npu-umd-test config file. All sections are described in the
[../validation/umd-test/configs/README.md documentation](../validation/umd-test/configs/README.md).
Below is an example of config file with models downloaded in previous section [Prepare a model](#prepare-a-model).

```yaml
model_dir: models/
image_dir: images/

graph_execution:
  - path: add_abc.xml
    # The GraphQueryNetwork* requires to pass any compiler acceptable flag.
    # TODO: Fix in the next release after v1.23.0
    flags: "--config"
  - path: resnet50.xml
    name: resnet50
    flags: "--config"
    in: [ dog.bmp ]
    class_index: [ 258 ]
  - path: yolov8s.xml
    name: yolov8s
    flags: "--config"

image_classification_imagenet:
  - path: resnet50.xml
    name: resnet50
    in: [ dog.bmp ]
    class_index: [ 258 ]

driver_cache:
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

Run all tests from the config file:

```bash
$ ./npu-umd-test --config=extend.yaml
```

### Run additional tests using options

The npu-umd-test comes with extra test cases:
* Driver initialization tests that requires to be run in new process
```bash
./npu-umd-test --ze-init-tests
```
* GPU and NPU tests using Level Zero API. Requires [compute-runtime](https://github.com/intel/compute-runtime/releases) to be installed 
```bash
./npu-umd-test --gpu
```
* External memory tests using System DMA Heap. Requires access to /dev/dma_heap/system that is limited to root access in Ubuntu
```bash
sudo ./npu-umd-test --dma-heap
```

### Reference test results

Table is filled with test results gathered using [v1.23.0 release](https://github.com/intel/linux-npu-driver/releases/tag/v1.23.0)

|Platform|System|Command|Test Result|Test Skipped|
|:---:|:---:|:---:|:---:|:---:|
|[Meteor Lake](https://ark.intel.com/content/www/us/en/ark/products/codename/90353/products-formerly-meteor-lake.html)|Ubuntu 24.04 6.16 development kernel|`npu-umd-test --config=extend.yaml`|276/291 passed|15 skipped|
