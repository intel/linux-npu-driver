/*
 * Copyright (C) 2023-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
#include "image.hpp"
#include "openvino/op/ops.hpp"
#include "openvino/openvino.hpp"
#include "openvino/runtime/intel_npu/level_zero/level_zero.hpp"
#include "openvino/runtime/intel_npu/properties.hpp"
#include "umd_dma_heap_system.hpp"
#include "umd_test.h"

#include <dlfcn.h>
#include <fcntl.h>
#include <filesystem>
#include <memory>
#include <sstream>
#include <sys/mman.h>

class OpenVinoTest : public UmdTest {
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

    bool isPluginCompilerInstalled() {
        Dl_info info = {};
        int ret = dladdr(reinterpret_cast<void *>(ov::get_openvino_version), &info);
        if (ret == 0) {
            std::cerr << "Failed to get OpenVINO library path" << std::endl;
            return false;
        }
        auto ovLibDir = std::filesystem::path(info.dli_fname).parent_path();
        return std::find(std::filesystem::directory_iterator(ovLibDir),
                         std::filesystem::directory_iterator(),
                         ovLibDir / "libopenvino_intel_npu_compiler_loader.so") !=
               std::filesystem::directory_iterator();
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

        return std::make_shared<ov::Model>(ov::OutputVector{relu},
                                           ov::ParameterVector{std::move(param)});
    }

    float refResult(float x, float c) {
        float res = x + c;
        return res > 0.0f ? res : 0.0f;
    }

    void inferCompiledBaseModel(ov::CompiledModel &compiledModel, const ov::Tensor &inTensor) {
        auto inferReq = compiledModel.create_infer_request();
        inferReq.set_input_tensor(inTensor);
        inferReq.infer();
        auto outTensor = inferReq.get_output_tensor();
        auto result = outTensor.data<float>();

        for (size_t i = 0; i < outTensor.get_size(); i++) {
            EXPECT_FLOAT_EQ(result[i], refResult(refInData[i], refConst[i]));
        }
    }

    void inferBaseModel(const std::shared_ptr<ov::Model> &model,
                        const ov::Tensor &inTensor,
                        ov::intel_npu::CompilerType compilerType) {
        auto compiledModel =
            core.compile_model(model, "NPU", ov::intel_npu::compiler_type(compilerType));
        inferCompiledBaseModel(compiledModel, inTensor);
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

class OpenVinoBasic : public OpenVinoTest,
                      public ::testing::WithParamInterface<ov::intel_npu::CompilerType> {
  public:
    void SetUp() override {
        OpenVinoTest::SetUp();

        if (getCompilerType() == ov::intel_npu::CompilerType::PLUGIN) {
            if (!isPluginCompilerInstalled()) {
                GTEST_SKIP() << "NPU plugin compiler is not installed";
            }
        }
    }
    ov::intel_npu::CompilerType getCompilerType() { return GetParam(); }
};

INSTANTIATE_TEST_SUITE_P(,
                         OpenVinoBasic,
                         ::testing::Values(ov::intel_npu::CompilerType::DRIVER,
                                           ov::intel_npu::CompilerType::PLUGIN),
                         testing::PrintToStringParamName());

class OpenVinoImageNet
    : public OpenVinoTest,
      public ::testing::WithParamInterface<std::tuple<ov::intel_npu::CompilerType, YAML::Node>> {
  public:
    void SetUp() override {
        OpenVinoTest::SetUp();

        if (getCompilerType() == ov::intel_npu::CompilerType::PLUGIN) {
            if (!isPluginCompilerInstalled()) {
                GTEST_SKIP() << "NPU plugin compiler is not installed";
            }
        }
    }
    ov::intel_npu::CompilerType getCompilerType() { return std::get<0>(GetParam()); }
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(OpenVinoImageNet);

INSTANTIATE_TEST_SUITE_P(
    ,
    OpenVinoImageNet,
    ::testing::Combine(::testing::Values(ov::intel_npu::CompilerType::DRIVER,
                                         ov::intel_npu::CompilerType::PLUGIN),
                       ::testing::ValuesIn(Environment::getConfiguration("openvino"))),
    [](const ::testing::TestParamInfo<std::tuple<ov::intel_npu::CompilerType, YAML::Node>> &p) {
        std::ostringstream name;
        name << generateTestNameFromNode(std::get<1>(p.param)) << "_" << std::get<0>(p.param);
        return name.str();
    });

TEST_P(OpenVinoImageNet, CompileModelWithGraphInitAndExecute) {
    const YAML::Node node = std::get<1>(GetParam());

    std::string modelPath(globalConfig.modelDir + node["path"].as<std::string>());
    auto mainModel = core.read_model(modelPath);
    ASSERT_NE(mainModel, nullptr);

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

TEST_P(OpenVinoBasic, RemoteHostTensorAsNpuModelInput) {
    auto model = createBaseModel();

    auto zeroContext = ov::intel_npu::level_zero::ZeroContext(core);
    auto inTensor = zeroContext.create_l0_host_tensor(model->input().get_element_type(),
                                                      model->input().get_shape());
    // Only NPU Remote Tensor allows to use `get()` method to access underlying buffer
    memcpy(inTensor.get(), refInData.data(), refInData.size() * sizeof(float));

    EXPECT_NO_THROW(inferBaseModel(model, inTensor, getCompilerType()));
}

using OpenVinoDmaHeap = OpenVinoBasic;

INSTANTIATE_TEST_SUITE_P(,
                         OpenVinoDmaHeap,
                         ::testing::Values(ov::intel_npu::CompilerType::DRIVER,
                                           ov::intel_npu::CompilerType::PLUGIN),
                         testing::PrintToStringParamName());

TEST_P(OpenVinoDmaHeap, RemoteTensorFromDmaBufAsNpuModelInput) {
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

    EXPECT_NO_THROW(inferBaseModel(model, inTensor, getCompilerType()));
}

TEST_P(OpenVinoBasic, RemoteTensorFromFileAsNpuModelInput) {
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

    EXPECT_NO_THROW(inferBaseModel(model, inTensor, getCompilerType()));
    unlink("input_tensor.bin");
}

class OpenVinoCache : public OpenVinoBasic {
  public:
    void SetUp() override {
        OpenVinoTest::SetUp();

        if (getCompilerType() == ov::intel_npu::CompilerType::PLUGIN) {
            if (!isPluginCompilerInstalled()) {
                GTEST_SKIP() << "NPU plugin compiler is not installed";
            }
        }

        std::filesystem::remove_all(ovCacheDirectory);
        std::filesystem::create_directories(ovCacheDirectory);
        std::filesystem::create_directories(driverCacheDirectory);
        clearDirectory(driverCacheDirectory);
    }

    void TearDown() override {
        std::filesystem::remove_all(ovCacheDirectory);
        OpenVinoTest::TearDown();
    }

    static void clearDirectory(const std::filesystem::path &dir) {
        if (!std::filesystem::is_directory(dir))
            return;
        for (auto const &file : std::filesystem::directory_iterator{dir})
            std::filesystem::remove(file);
    }

    static size_t countFiles(const std::filesystem::path &dir) {
        if (!std::filesystem::is_directory(dir))
            return 0;
        return static_cast<size_t>(std::distance(std::filesystem::directory_iterator{dir},
                                                 std::filesystem::directory_iterator{}));
    }

    const std::filesystem::path driverCacheDirectory = []() {
        const char *env = getenv("HOME");
        if (env)
            return std::filesystem::path(env) / ".cache/ze_intel_npu_cache";
        return std::filesystem::current_path() / ".cache/ze_intel_npu_cache";
    }();
    const std::filesystem::path ovCacheDirectory =
        std::filesystem::temp_directory_path() / "ov_cache_test";
};

INSTANTIATE_TEST_SUITE_P(,
                         OpenVinoCache,
                         ::testing::Values(ov::intel_npu::CompilerType::DRIVER,
                                           ov::intel_npu::CompilerType::PLUGIN),
                         testing::PrintToStringParamName());

TEST_P(OpenVinoCache, DriverCacheIsUsed) {
    const auto compilerType = GetParam();
    auto model = createBaseModel();

    auto inTensor = ov::Tensor(model->input().get_element_type(), model->input().get_shape());
    memcpy(inTensor.data<float>(), refInData.data(), refInData.size() * sizeof(float));

    auto expectOvCacheFiles = 0u;
    auto expectDriverCacheFiles = compilerType == ov::intel_npu::CompilerType::DRIVER ? 1u : 0u;
    ov::CompiledModel compiledModel;

    ASSERT_NO_THROW(
        compiledModel =
            core.compile_model(model, "NPU", ov::intel_npu::compiler_type(compilerType)));
    EXPECT_NO_THROW(inferCompiledBaseModel(compiledModel, inTensor));
    EXPECT_EQ(compiledModel.get_property(ov::loaded_from_cache), false);
    EXPECT_EQ(countFiles(ovCacheDirectory), expectOvCacheFiles);
    EXPECT_EQ(countFiles(driverCacheDirectory), expectDriverCacheFiles);

    auto driverCacheWriteTime = std::filesystem::file_time_type::min();
    if (compilerType == ov::intel_npu::CompilerType::DRIVER) {
        driverCacheWriteTime = std::filesystem::last_write_time(
            *std::filesystem::directory_iterator{driverCacheDirectory});
    }

    ASSERT_NO_THROW(
        compiledModel =
            core.compile_model(model, "NPU", ov::intel_npu::compiler_type(compilerType)));
    EXPECT_NO_THROW(inferCompiledBaseModel(compiledModel, inTensor));
    EXPECT_EQ(compiledModel.get_property(ov::loaded_from_cache), false);
    EXPECT_EQ(countFiles(ovCacheDirectory), expectOvCacheFiles);
    EXPECT_EQ(countFiles(driverCacheDirectory), expectDriverCacheFiles);

    if (compilerType == ov::intel_npu::CompilerType::DRIVER) {
        EXPECT_EQ(std::filesystem::last_write_time(
                      *std::filesystem::directory_iterator{driverCacheDirectory}),
                  driverCacheWriteTime);
    }
}

TEST_P(OpenVinoCache, OvCacheIsUsed) {
    const auto compilerType = GetParam();
    auto model = createBaseModel();
    core.set_property(ov::cache_dir(ovCacheDirectory.string()));

    auto inTensor = ov::Tensor(model->input().get_element_type(), model->input().get_shape());
    memcpy(inTensor.data<float>(), refInData.data(), refInData.size() * sizeof(float));

    auto expectOvCacheFiles = 1u;
    auto expectDriverCacheFiles = 0;
    ov::CompiledModel compiledModel;

    ASSERT_NO_THROW(
        compiledModel =
            core.compile_model(model, "NPU", ov::intel_npu::compiler_type(compilerType)));
    EXPECT_NO_THROW(inferCompiledBaseModel(compiledModel, inTensor));
    EXPECT_EQ(compiledModel.get_property(ov::loaded_from_cache), false);
    ASSERT_EQ(countFiles(ovCacheDirectory), expectOvCacheFiles);
    EXPECT_EQ(countFiles(driverCacheDirectory), expectDriverCacheFiles);

    auto ovCacheWriteTime =
        std::filesystem::last_write_time(*std::filesystem::directory_iterator{ovCacheDirectory});

    ASSERT_NO_THROW(
        compiledModel =
            core.compile_model(model, "NPU", ov::intel_npu::compiler_type(compilerType)));
    EXPECT_NO_THROW(inferCompiledBaseModel(compiledModel, inTensor));
    EXPECT_EQ(compiledModel.get_property(ov::loaded_from_cache), true);
    ASSERT_EQ(countFiles(ovCacheDirectory), expectOvCacheFiles);
    EXPECT_EQ(countFiles(driverCacheDirectory), expectDriverCacheFiles);
    EXPECT_EQ(
        std::filesystem::last_write_time(*std::filesystem::directory_iterator{ovCacheDirectory}),
        ovCacheWriteTime);
}
