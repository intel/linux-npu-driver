/*
 * Copyright (C) 2023-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
#include "umd_test.h"
#include "openvino/openvino.hpp"
#include "image.hpp"

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
    }

    template <auto elementType>
    void processResults(const ov::Tensor &inferenceOutput, uint32_t classExpected) {
        using T = ov::fundamental_type_for<elementType>;
        ov::Shape outputShape = inferenceOutput.get_shape();
        ASSERT_GT(outputShape.size(), 0);

        /* Only batch size 1 is processed */
        ASSERT_EQ(outputShape[0], 1);

        std::vector<T> results(inferenceOutput.get_size());
        const T *outputData = inferenceOutput.data<const T>();
        std::for_each(results.begin(), results.end(), [&](T &n) { n = *outputData++; });
        auto it = std::max_element(results.begin(), results.end());
        size_t index = std::distance(results.begin(), it);
        PRINTF("Class = %li, Accuracy = %f\n", index, static_cast<float>(*it));
        ASSERT_EQ(index, static_cast<size_t>(classExpected));
    }

    void processResults(const ov::Tensor &results, uint32_t exp) {
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
};

TEST_F(OpenVinoBasic, CheckDevice) {
    try {
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
    } catch (const std::exception &e) {
        GTEST_FAIL() << "Exception: " << e.what() << std::endl;
    }
}

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
        Image img(testImages[i]);

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
        if (networkOutputs.size() == 1 &&
            networkOutputs[0].get_shape() == ov::Shape(std::initializer_list<size_t>({1, 1000})) &&
            !expectedImageClassIndexes.empty()) {
            const ov::Tensor &output = infer_request.get_output_tensor();
            processResults(output, expectedImageClassIndexes[i]);
        } else {
            for (size_t index = 0; index < networkOutputs.size(); index++) {
                const ov::Tensor &output = infer_request.get_output_tensor(index);
                ASSERT_GT(output.get_size(), 0);
            }
        }
    }
}
