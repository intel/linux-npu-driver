<!-- Copyright 2022-2024 Intel Corporation.

 This software and the related documents are Intel copyrighted materials, and
 your use of them is governed by the express license under which they were
 provided to you ("License"). Unless the License provides otherwise, you may
 not use, modify, copy, publish, distribute, disclose or transmit this
 software or the related documents without Intel's prior written permission.

 This software and the related documents are provided as is, with no express
 or implied warranties, other than those that are expressly stated in
 the License. -->


# Models

The Linux driver defines intel-validation-models-npu package that contains
models used by various test app, ex. benchmark_app, npu-umd-test, protopipe,
etc.. The link to this package can be found in bkc_components.vars in entry
`NPU_{TARGET_DISTRO}_MODELS_PKG_URL`.

To update the package with models, user has to set new archive in
NPU_MODELS_PUBLIC_URL field, adjust the validation/models/CMakeLists.txt in
case it was necessary to exclude some models from the archive and then trigger
["Models" workflow in Linux CI GitHub
Actions](https://github.com/intel-innersource/drivers.vpu.linux.client/actions/workflows/models.yml).
Using artifactory link from "Models" workflow, user can exchange the
`NPU_{TARGET_DISTRO}_MODELS_PKG_URL` in bkc_components.vars and then merge it
to main branch.

`add_abc/add_abc.xml` is copied from OpenVINO repository:
https://github.com/openvinotoolkit/openvino/blob/adeb3d2e0296db45a745ad6c02d4566570b18750/src/core/tests/models/ir/add_abc.xml
