/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/ext/compiler.hpp"
#include "level_zero_driver/source/ext/graph.hpp"
#include "level_zero_driver/unit_tests/fixtures/graph_fixture.hpp"
#include "npu_driver_compiler.h"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <string>
#include <ze_api.h>
#include <ze_graph_ext.h>

namespace L0 {
namespace ult {

using CompilerInDriver = Test<GraphNGraphLiteFixture>;

TEST_F(CompilerInDriver, versionCheck) {
    EXPECT_EQ(Compiler::getVclCompilerApiVersion().major, VCL_COMPILER_VERSION_MAJOR);
    EXPECT_EQ(Compiler::getVclCompilerApiVersion().minor, VCL_COMPILER_VERSION_MINOR);
    EXPECT_TRUE(Compiler::isVclCompilerApiCompatible());
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithNullInputReturnsFailure) {
    graphDesc.pInput = nullptr;

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, res);
    ASSERT_EQ(nullptr, hGraph);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithInputSizeZeroReturnsFailure) {
    graphDesc.inputSize = 0u;

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_ERROR_INVALID_SIZE, res);
    ASSERT_EQ(nullptr, hGraph);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithNullBuildOptionsReturnsSuccess) {
    graphDesc.pBuildFlags = nullptr;

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);

    ze_graph_properties_3_t graphProps;
    EXPECT_EQ(L0::Graph::fromHandle(hGraph)->getProperties3(&graphProps), ZE_RESULT_SUCCESS);
    EXPECT_EQ(graphProps.numGraphArgs, 4u);
    EXPECT_EQ(graphProps.initStageRequired, ZE_GRAPH_STAGE_INITIALIZE);
    EXPECT_EQ(graphProps.flags, ZE_GRAPH_PROPERTIES_FLAG_COMPILED);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithNoBuildOptionsReturnsSuccess) {
    graphDesc.pBuildFlags = "";

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithBuildOptionsReturnsSuccess) {
    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_EQ(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithPerfCountConfigReturnsSuccess) {
    if (buildFlags.find("--config") == std::string::npos)
        buildFlags += " --config";
    buildFlags += " PERF_COUNT=\"YES\"";
    graphDesc.pBuildFlags = buildFlags.c_str();

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_GT(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithEnableProfilingFlagReturnsSuccess) {
    graphDesc.flags = ZE_GRAPH_FLAG_ENABLE_PROFILING;

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_GT(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithEnableProfilingFlagAndLogLevelConfigReturnsSuccess) {
    if (buildFlags.find("--config") == std::string::npos)
        buildFlags += " --config";
    buildFlags += " LOG_LEVEL=\"LOG_ERROR\"";
    graphDesc.pBuildFlags = buildFlags.c_str();

    graphDesc.flags = ZE_GRAPH_FLAG_ENABLE_PROFILING;
    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_GT(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

TEST_F(CompilerInDriver,
       creatingNgraphLiteWithEnableProfilingFlagAndPerfCountConfigReturnsSuccess) {
    if (buildFlags.find("--config") == std::string::npos)
        buildFlags += " --config";
    buildFlags += " PERF_COUNT=\"YES\"";
    graphDesc.pBuildFlags = buildFlags.c_str();

    graphDesc.flags = ZE_GRAPH_FLAG_ENABLE_PROFILING;
    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_GT(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

} // namespace ult
} // namespace L0
