/*
 * Copyright (C) 2023-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
#include "image.hpp"
#include "openvino/op/ops.hpp"
#include "openvino/openvino.hpp"
#include "openvino/runtime/intel_npu/level_zero/level_zero.hpp"
#include "umd_dma_heap_system.hpp"
#include "umd_test.h"

#include <fcntl.h>
#include <memory>
#include <sys/mman.h>

class OpenVinoBasic : public UmdTest, public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        UmdTest::SetUp();
        YAML::Node &configuration = Environment::getConfiguration();
        if (configuration["ov_log_level"].IsDefined()) {
            std::string lvl = configuration["ov_log_level"].as<std::string>();
            ov::log::Level logLevel;
            if (lvl == "ERR")
                logLevel = ov::log::Level::ERR;
            else if (lvl == "WARNING")
                logLevel = ov::log::Level::WARNING;
            else if (lvl == "INFO")
                logLevel = ov::log::Level::INFO;
            else if (lvl == "DEBUG")
                logLevel = ov::log::Level::DEBUG;
            else if (lvl == "TRACE")
                logLevel = ov::log::Level::TRACE;
            else
                logLevel = ov::log::Level::NO;

            core.set_property(ov::log::level(logLevel));
        }

        ov::Version version = ov::get_openvino_version();
        TRACE("OpenVINO name:      %s\n", version.description);
        TRACE("OpenVINO build:     %s\n", version.buildNumber);

        const std::vector<std::string> devices = core.get_available_devices();
        for (auto &&device : devices) {
            if (device == "NPU") {
                auto deviceVersion = core.get_versions(device);
                for (auto &v : deviceVersion) {
                    TRACE("Plugin name:        %s\n", v.second.description);
                    TRACE("Plugin build:       %s\n", v.second.buildNumber);
                }
                return;
            }
        }
        GTEST_FAIL() << "NPU device not available" << std::endl;
    }

    auto createBaseModel() {
        // Model:
        //  Parameter(1,3) --> Add --> Relu
        //      Constant(1,3) /
        // Example results:
        // * [0.0, 0.0, 0.0]    --> [0.5, 1.0, 0.0]
        // * [1.0, 1.0, 1.0]    --> [1.5, 2.0, 0.0]
        // * [-1.0, -1.0, -1.0] --> [0.0, 0.0, 0.0]
        auto param = std::make_shared<ov::op::v0::Parameter>(ov::element::f32, ov::Shape{1, 3});
        auto constant = ov::op::v0::Constant::create(ov::element::f32, ov::Shape{1, 3}, refConst);
        auto add = std::make_shared<ov::op::v1::Add>(param, constant);
        auto relu = std::make_shared<ov::op::v0::Relu>(add);

        return std::make_shared<ov::Model>(ov::NodeVector{relu},
                                           ov::ParameterVector{std::move(param)});
    }

    float refResult(float x, float c) {
        float res = x + c;
        return res > 0.0f ? res : 0.0f;
    }

    void inferBaseModel(const std::shared_ptr<ov::Model> &model, const ov::Tensor &inTensor) {
        auto compiledModel = core.compile_model(model, "NPU");
        auto inferReq = compiledModel.create_infer_request();
        inferReq.set_input_tensor(inTensor);
        inferReq.infer();
        auto outTensor = inferReq.get_output_tensor();
        auto result = outTensor.data<float>();

        for (size_t i = 0; i < outTensor.get_size(); i++) {
            EXPECT_FLOAT_EQ(result[i], refResult(refInData[i], refConst[i]));
        }
    }

    template <auto elementType>
    void processResults(ov::Tensor &inferenceOutput, uint32_t classExpected) {
        using T = ov::fundamental_type_for<elementType>;
        ov::Shape outputShape = inferenceOutput.get_shape();
        ASSERT_GT(outputShape.size(), 0);

        /* Only batch size 1 is processed */
        ASSERT_EQ(outputShape[0], 1);

        std::vector<T> results(inferenceOutput.get_size());
        T *outputData = inferenceOutput.data<T>();
        std::for_each(results.begin(), results.end(), [&](T &n) { n = *outputData++; });
        auto it = std::max_element(results.begin(), results.end());
        size_t index = std::distance(results.begin(), it);
        PRINTF("Class = %li, Accuracy = %f\n", index, static_cast<float>(*it));
        ASSERT_EQ(index, static_cast<size_t>(classExpected));
    }

    void processResults(ov::Tensor &results, uint32_t exp) {
        using T = ov::element::Type_t;
        switch (results.get_element_type()) {
        case T::f32:
            processResults<T::f32>(results, exp);
            break;
        case T::f64:
            processResults<T::f64>(results, exp);
            break;
        case T::f16:
            processResults<T::f16>(results, exp);
            break;
        case T::i16:
            processResults<T::i16>(results, exp);
            break;
        case T::u8:
            processResults<T::u8>(results, exp);
            break;
        case T::i8:
            processResults<T::i8>(results, exp);
            break;
        case T::u16:
            processResults<T::u16>(results, exp);
            break;
        case T::i32:
            processResults<T::i32>(results, exp);
            break;
        case T::u32:
            processResults<T::u32>(results, exp);
            break;
        case T::i64:
            processResults<T::i64>(results, exp);
            break;
        case T::u64:
            processResults<T::u64>(results, exp);
            break;
        default:
            GTEST_FAIL() << "Unrecognized result type" << std::endl;
            break;
        }
    }

    ov::Core core;
    // Reference input data for base model tests
    const std::array<float, 3> refInData = {1.0f, 1.0f, 1.0f};
    const std::vector<float> refConst = {0.5f, 1.0f, -1.f};
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(OpenVinoBasic);

INSTANTIATE_TEST_SUITE_P(,
                         OpenVinoBasic,
                         ::testing::ValuesIn(Environment::getConfiguration("openvino")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(OpenVinoBasic, CompileModelWithGraphInitAndExecute) {
    const YAML::Node node = GetParam();

    std::string modelPath(globalConfig.modelDir + node["path"].as<std::string>());
    auto mainModel = core.read_model(modelPath.c_str());

    ASSERT_EQ(mainModel->inputs().size(), 1);

    std::vector<std::string> testImages;
    for (auto &image : node["in"].as<std::vector<std::string>>()) {
        testImages.push_back(globalConfig.imageDir + image);
    }

    if (testImages.empty())
        GTEST_FAIL() << "Test image not found" << std::endl;
    std::vector<uint16_t> expectedImageClassIndexes;

    if (node["class_index"].IsDefined()) {
        expectedImageClassIndexes = node["class_index"].as<std::vector<uint16_t>>();
        ASSERT_EQ(expectedImageClassIndexes.size(), testImages.size());
    }

    for (size_t i = 0; i < testImages.size(); i++) {
        /* Use default BGR order for color channels*/
        Image img(testImages[i], false);

        /* This test is limited to images with one byte per channel -> 3 bytes per pixel */
        /* TODO: Add support for other pixel data formats */
        ASSERT_EQ(img.getElementSize(), sizeof(ov::fundamental_type_for<ov::element::u8>));

        ov::element::Type input_type = ov::element::u8;
        const ov::Layout input_layout{"NHWC"};
        ov::Shape input_shape = {1,
                                 static_cast<uint32_t>(img.getHeight()),
                                 static_cast<uint32_t>(img.getWidth()),
                                 static_cast<uint32_t>(img.getChannels())};
        ov::Tensor input_tensor = ov::Tensor(input_type, input_shape, img.getPtr());

        auto networkOutputs = mainModel->outputs();
        ov::preprocess::PrePostProcessor ppp(mainModel);

        ppp.input()
            .tensor()
            .set_shape(input_shape)
            .set_element_type(input_type)
            .set_layout(input_layout);
        ppp.input().preprocess().resize(ov::preprocess::ResizeAlgorithm::RESIZE_LINEAR);
        for (auto &o : networkOutputs)
            ppp.output(o.get_any_name()).tensor().set_element_type(ov::element::f32);
        auto model = ppp.build();

        ov::CompiledModel compiled_model = core.compile_model(model, "NPU");
        ov::InferRequest infer_request = compiled_model.create_infer_request();

        infer_request.set_input_tensor(input_tensor);
        infer_request.infer();

        /* Test can parse and compare results for imagenet classes where
         * output format has shape: [ 1, 1000 ]
         */
        ov::Tensor output;
        if (networkOutputs.size() == 1 &&
            networkOutputs[0].get_shape() == ov::Shape(std::initializer_list<size_t>({1, 1000})) &&
            !expectedImageClassIndexes.empty()) {
            output = infer_request.get_output_tensor();
            processResults(output, expectedImageClassIndexes[i]);
        } else {
            for (size_t index = 0; index < networkOutputs.size(); index++) {
                output = infer_request.get_output_tensor(index);
                ASSERT_GT(output.get_size(), 0);
            }
        }
    }
}

TEST_F(OpenVinoBasic, RemoteHostTensorAsNpuModelInput) {
    auto model = createBaseModel();

    auto zeroContext = ov::intel_npu::level_zero::ZeroContext(core);
    auto inTensor = zeroContext.create_l0_host_tensor(model->input().get_element_type(),
                                                      model->input().get_shape());
    // Only NPU Remote Tensor allows to use `get()` method to access underlying buffer
    memcpy(inTensor.get(), refInData.data(), refInData.size() * sizeof(float));

    inferBaseModel(model, inTensor);
}

using OpenVinoDmaHeap = OpenVinoBasic;

TEST_F(OpenVinoDmaHeap, RemoteTensorFromDmaBufAsNpuModelInput) {
    DmaHeapSystem dmaHeapSystem;
    CHECK_DMA_HEAP_SUPPORT(dmaHeapSystem);

    auto model = createBaseModel();

    auto dmaBufSize =
        model->input().get_element_type().size() * ov::shape_size(model->input().get_shape());
    auto dmaBuf = dmaHeapSystem.allocDmaHeapBuffer(dmaBufSize);
    ASSERT_NE(dmaBuf, nullptr);

    auto zeroContext = ov::intel_npu::level_zero::ZeroContext(core);
    auto inTensor = zeroContext.create_tensor(model->input().get_element_type(),
                                              model->input().get_shape(),
                                              dmaBuf->fd);
    // Only NPU Remote Tensor allows to use `get()` method to access underlying buffer
    memcpy(inTensor.get(), refInData.data(), refInData.size() * sizeof(float));

    inferBaseModel(model, inTensor);
}

TEST_F(OpenVinoBasic, RemoteTensorFromFileAsNpuModelInput) {
    auto model = createBaseModel();

    auto fd = open("input_tensor.bin", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    ASSERT_NE(fd, -1) << "Failed to create file for mmap tensor";
    std::shared_ptr<int> fdPtr(&fd, [fd](auto) { close(fd); });

    auto fileSize = refInData.size() * sizeof(float);
    ASSERT_EQ(write(fd, refInData.data(), fileSize), static_cast<ssize_t>(fileSize))
        << "Failed to write data to mmap tensor file";

    // Pass FileDescriptor forces the plugin to import system memory to NPU.
    // If import fails, the plugin falls back to use driver host allocation
    ov::intel_npu::FileDescriptor fileDescriptor{"input_tensor.bin", 0};
    auto zeroContext = ov::intel_npu::level_zero::ZeroContext(core);
    auto inTensor = zeroContext.create_tensor(model->input().get_element_type(),
                                              model->input().get_shape(),
                                              fileDescriptor);

    inferBaseModel(model, inTensor);
    unlink("input_tensor.bin");
}
