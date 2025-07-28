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
git clone https://github.com/intel/linux-npu-driver.git
cd linux-npu-driver/

git submodule update --init --recursive

cmake -B build -S .
cmake --build build/ -j11 --target npu-umd-test

ls build/bin/
./build/bin/npu-umd-test --gtest_filter=Device.GetProperties*
```

## Prepare a model

The NPU driver supports models in the OpenVINO IR format. OpenVINO allows to convert model from
different frameworks, see [Conventional Model Preparation](https://docs.openvino.ai/2025/openvino-workflow/model-preparation.html) docs. 

The npu-umd-test is able to run tests with single model or with a set of models. The models have to be converted to the OpenVINO IR format.

### Example: Image Classification Model

The npu-umd-test allows to test output of image classification model trained on ImageNet dataset. Below is an
example of how to download and convert a ResNet-50 model from Pytorch to OpenVINO IR format.

```bash
python3 -m venv openvino-venv
source openvino-venv/bin/activate
pip install --upgrade pip
pip install openvino torch torchvision opencv-python
# check time of installation with '--extra-index-url=https://download.pytorch.org/whl/cpu'
```

```python 
import openvino
import os
import torch
import torchvision

os.makedirs("models", exist_ok=True)
model = torchvision.models.resnet50(weights='DEFAULT')
# Force static shape input. Dynamic shape is not fully supported by the NPU driver
ov_model = openvino.convert_model(model, example_input=torch.rand(1, 3, 224, 224))

ppp = openvino.preprocess.PrePostProcessor(ov_model)
ppp.input().tensor() \
    .set_shape([1,224,224,3]) \
    .set_element_type(openvino.Type.u8) \
    .set_layout(openvino.Layout('NHWC'))
ppp.input().model().set_layout(openvino.Layout('NCHW'))
ppp.output().tensor() \
    .set_element_type(openvino.Type.f32)
ov_model = ppp.build()
openvino.save_model(ov_model, "models/resnet50.xml")

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

Add the model and the image to the new npu-umd-test config file:
```bash
$ cat <<EOF > extend.yaml

model_dir: models/
image_dir: images/

image_classification_imagenet:
  - path: resnet50.xml
    name: resnet50
    in: [ dog.bmp ]
    class_index: [ 284 ]
EOF

# Test the change
$ ./npu-umd-test --config=basic.yaml CompilerInDriverLongBmp.CompileModelWithGraphInitAndExecuteThenCheckAccuracy/resnet50 -v
```

### Example: Object Detection Model

TBD

### Run tests

There is multiple section in the npu-umd-test config file. All sections are described in the
[../validation/umd-test/configs/README.md documentation](../validation/umd-test/configs/README.md).
Below is an example of config file with models downloaded in previous section [Prepare a model](#prepare-a-model).

```yaml
TBD
```

### Reference test results

TBD

|Platform|System|Test Result|Test Skipped|
|:---:|:---:|:---:|:---|
|[Meteor Lake](https://ark.intel.com/content/www/us/en/ark/products/codename/90353/products-formerly-meteor-lake.html)|Ubuntu 24.04 HWE 6.11.0-26-generic|X/X passed|X skipped|
 