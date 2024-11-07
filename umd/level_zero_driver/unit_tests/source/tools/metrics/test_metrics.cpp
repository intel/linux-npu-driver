/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "gtest/gtest.h"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include "level_zero_driver/tools/source/metrics/metric_query.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "level_zero_driver/unit_tests/mocks/mock_metrics.hpp"
#include "umd_common.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>
#include <memory>
#include <string.h>
#include <string>
#include <vector>

namespace L0 {
namespace ult {

using MetricNonEnvTest = Test<DeviceFixtureWithoutEnvVariables>;

TEST_F(MetricNonEnvTest, metricGroupGetReturnsFailureWithoutEnvVariables) {
    uint32_t count = 0;
    EXPECT_EQ(device->metricGroupGet(&count, nullptr), ZE_RESULT_ERROR_UNINITIALIZED);
}

using MetricTest = Test<DeviceFixture>;

TEST_F(MetricTest, metricGroupGetReturnsExpectedResultsWithInitializedDevice) {
    EXPECT_EQ(device->metricGroupGet(nullptr, nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    uint32_t count = 0;
    EXPECT_EQ(device->metricGroupGet(&count, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_GT(count, 0u);

    std::vector<zet_metric_group_handle_t> metricGroups(count);
    EXPECT_EQ(*metricGroups.data(), nullptr);
    EXPECT_EQ(device->metricGroupGet(&count, metricGroups.data()), ZE_RESULT_SUCCESS);
    EXPECT_NE(*metricGroups.data(), nullptr);
}

using MetricContextTest = Test<ContextFixture>;

TEST_F(MetricContextTest, metricContextActivateMetricGroupsReturnsFailureWithoutInitializedDevice) {
    auto deviceNew = std::make_unique<Device>(nullptr, nullptr);

    ASSERT_NE(context, nullptr);
    EXPECT_EQ(context->activateMetricGroups(deviceNew->toHandle(), 0, nullptr),
              ZE_RESULT_ERROR_UNINITIALIZED);
}

struct MetricGroupShared : public ContextFixture {
    void SetUp() override {
        ContextFixture::SetUp();

        ASSERT_EQ(device->metricGroupGet(&metricGroupCount, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_GT(metricGroupCount, 0u);

        metricGroups.resize(metricGroupCount);
        ASSERT_EQ(device->metricGroupGet(&metricGroupCount, metricGroups.data()),
                  ZE_RESULT_SUCCESS);
        ASSERT_NE(*metricGroups.data(), nullptr);
    }

    void TearDown() override {
        if (context != nullptr && device != nullptr) {
            // Deactivate all metric groups previously activated
            ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 0, nullptr),
                      ZE_RESULT_SUCCESS);

            auto mockMetricContext =
                reinterpret_cast<MockMetricContext *>(device->getMetricContext().get());

            // Confirming that no metric groups are activated
            EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 0u);
        }

        ContextFixture::TearDown();
    }

    void checkMetricsData(std::string metricGroupName,
                          std::vector<std::string> &metricNames,
                          std::vector<zet_metric_type_t> &metricTypes,
                          std::vector<zet_value_type_t> &valueTypes) {
        ASSERT_GT(metricGroups.size(), 0u);
        ASSERT_GT(metricGroupCount, 0u);

        groupProperties.resize(metricGroupCount);
        int metricGroupIndex = -1;
        uint32_t metricCount = 0;

        for (uint8_t i = 0; i < metricGroupCount; i++) {
            ASSERT_EQ(
                L0::MetricGroup::fromHandle(metricGroups[i])->getProperties(&groupProperties[i]),
                ZE_RESULT_SUCCESS);
            if (memcmp(groupProperties[i].name,
                       metricGroupName.c_str(),
                       metricGroupName.length()) == 0) {
                EXPECT_TRUE((groupProperties[i].samplingType &
                             ZET_METRIC_GROUP_SAMPLING_TYPE_FLAG_EVENT_BASED) &&
                            (groupProperties[i].samplingType &
                             ZET_METRIC_GROUP_SAMPLING_TYPE_FLAG_TIME_BASED));
                metricGroupIndex = i;
                metricCount = groupProperties[i].metricCount;
                break;
            }
        }

        ASSERT_GT(metricGroupIndex, -1);
        ASSERT_EQ(metricCount, metricNames.size());

        std::vector<zet_metric_handle_t> metrics(metricCount);

        ASSERT_EQ(L0::MetricGroup::fromHandle(metricGroups[safe_cast<size_t>(metricGroupIndex)])
                      ->getMetric(&metricCount, metrics.data()),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(metricCount, metrics.size());

        std::vector<zet_metric_properties_t> properties(metricCount);

        for (uint8_t i = 0; i < metricCount; i++) {
            EXPECT_EQ(L0::Metric::fromHandle(metrics[i])->getProperties(&properties[i]),
                      ZE_RESULT_SUCCESS);

            EXPECT_EQ(memcmp(properties[i].name, metricNames[i].c_str(), metricNames[i].length()),
                      0);
            EXPECT_EQ(properties[i].metricType, metricTypes[i]);
            EXPECT_EQ(properties[i].resultType, valueTypes[i]);
        }
    }

    uint32_t metricGroupCount = 0;
    std::vector<zet_metric_group_handle_t> metricGroups;
    std::vector<zet_metric_group_properties_t> groupProperties;

    zet_metric_query_pool_desc_t queryPoolDesc = {.stype =
                                                      ZET_STRUCTURE_TYPE_METRIC_QUERY_POOL_DESC,
                                                  .pNext = nullptr,
                                                  .type = ZET_METRIC_QUERY_POOL_TYPE_PERFORMANCE,
                                                  .count = 1u};
};

struct MetricGroupTest : public Test<MetricGroupShared> {
    ze_context_handle_t hContext = nullptr;
    zet_metric_query_pool_handle_t hMetricQueryPool = nullptr;
};

TEST_F(MetricGroupTest, metricGroupGetPropertiesReturnsFailureWithIncorrectInput) {
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])->getProperties(nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
}

TEST_F(MetricGroupTest, metricGettingReturnsExpectedResults) {
    // Retrieve metrics for metric group #1
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])->getMetric(nullptr, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    uint32_t count = 0;
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])->getMetric(&count, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_GT(count, 0u);

    std::vector<zet_metric_handle_t> metrics(count);
    EXPECT_EQ(*metrics.data(), nullptr);
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])->getMetric(&count, metrics.data()),
              ZE_RESULT_SUCCESS);
    EXPECT_NE(*metrics.data(), nullptr);
}

TEST_F(MetricGroupTest, metricGetPropertiesReturnsExpectedResultsForNOCGroup) {
    // Populate expected data
    std::vector<std::string> metricNames = {"noc"};
    std::vector<zet_metric_type_t> metricTypes(1, ZET_METRIC_TYPE_EVENT);
    std::vector<zet_value_type_t> valueTypes(1, ZET_VALUE_TYPE_UINT64);

    checkMetricsData("NOC", metricNames, metricTypes, valueTypes);
}

TEST_F(MetricGroupTest, metricContextActivateMetricGroupsReturnsExpectedResults) {
    EXPECT_EQ(context->activateMetricGroups(nullptr, 0, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_HANDLE);

    EXPECT_EQ(context->activateMetricGroups(device->toHandle(), 0, nullptr), ZE_RESULT_SUCCESS);

    EXPECT_EQ(context->activateMetricGroups(device->toHandle(), 0, metricGroups.data()),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(context->activateMetricGroups(device->toHandle(), 1, nullptr),
              ZE_RESULT_ERROR_INVALID_SIZE);
}

TEST_F(MetricGroupTest, activateMetricGroupsReturnsProperExpectedActivatedGroups) {
    auto metricContext = device->getMetricContext();
    auto mockMetricContext = reinterpret_cast<MockMetricContext *>(metricContext.get());

    EXPECT_EQ(
        context->activateMetricGroups(device->toHandle(), metricGroupCount, metricGroups.data()),
        ZE_RESULT_SUCCESS);

    // Confirming that only 1 metric group is activated
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1u);

    // Attempting to pass in count > 0 with phMetricGroups being NULL
    ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 1, nullptr),
              ZE_RESULT_ERROR_INVALID_SIZE);

    // Confirming that metric groups are still activated
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1u);

    // Deactivating metric groups successfully
    EXPECT_EQ(context->activateMetricGroups(device->toHandle(), 0, nullptr), ZE_RESULT_SUCCESS);

    // Confirming that no metric groups are activated
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 0ul);

    // Activating only 1 metric group returns success
    EXPECT_EQ(context->activateMetricGroups(device->toHandle(), 1u, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    // Confirming that only 1 metric group is activated
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1ul);

    // Activating 1 specific metric group - deactivates the rest not passed in
    EXPECT_EQ(context->activateMetricGroups(device->toHandle(), 1u, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    // Confirming that only 1 metric group is still activated
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1ul);

    // Deactivating all metric groups
    EXPECT_EQ(context->activateMetricGroups(device->toHandle(), 0u, nullptr), ZE_RESULT_SUCCESS);

    // Confirming that no metric groups are activated
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 0ul);
}

TEST_F(MetricGroupTest, multipleContextActivateMetricGroupsIsolationTest) {
    L0::Context *context1 = nullptr;
    // Creating context1
    {
        ze_context_desc_t desc = {};
        ASSERT_EQ(driverHandle->createContext(&desc, &hContext), ZE_RESULT_SUCCESS);
        ASSERT_NE(nullptr, hContext);
        context1 = Context::fromHandle(hContext);
        ASSERT_NE(nullptr, context1);
    }

    L0::Context *context2 = nullptr;
    // Creating context2
    {
        ze_context_desc_t desc = {};
        ASSERT_EQ(driverHandle->createContext(&desc, &hContext), ZE_RESULT_SUCCESS);
        ASSERT_NE(nullptr, hContext);
        context2 = Context::fromHandle(hContext);
        ASSERT_NE(nullptr, context2);
    }

    auto metricContext = device->getMetricContext();
    auto mockMetricContext = reinterpret_cast<MockMetricContext *>(metricContext.get());
    ASSERT_NE(mockMetricContext, nullptr);

    // Activating metric group 1 using context1
    ASSERT_EQ(context1->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    // Confirming that only 1 metric group is activated for metricContext
    ASSERT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1ul);

    // Activating invalid metric groups combination for context2, expect failure
    ASSERT_EQ(context2->activateMetricGroups(device->toHandle(), 3, nullptr),
              ZE_RESULT_ERROR_INVALID_SIZE);

    // Confirming that no additional metric groups have been activated
    ASSERT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1ul);

    // Deactivating custom context metric group activation
    ASSERT_EQ(context1->activateMetricGroups(device->toHandle(), 0u, nullptr), ZE_RESULT_SUCCESS);

    ASSERT_EQ(context1->destroy(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(context2->destroy(), ZE_RESULT_SUCCESS);
}

TEST_F(MetricGroupTest, multipleContextActivateSpecificMetricGroupsHasUniqueControl) {
    L0::Context *context1 = nullptr;
    // Creating context1
    {
        ze_context_desc_t desc = {};
        ASSERT_EQ(driverHandle->createContext(&desc, &hContext), ZE_RESULT_SUCCESS);
        ASSERT_NE(nullptr, hContext);
        context1 = L0::Context::fromHandle(hContext);
        ASSERT_NE(nullptr, context1);
    }

    L0::Context *context2 = nullptr;
    // Creating context2
    {
        ze_context_desc_t desc = {};
        ASSERT_EQ(driverHandle->createContext(&desc, &hContext), ZE_RESULT_SUCCESS);
        ASSERT_NE(nullptr, hContext);
        context2 = L0::Context::fromHandle(hContext);
        ASSERT_NE(nullptr, context2);
    }

    L0::Context *context3 = nullptr;
    // Creating context3
    {
        ze_context_desc_t desc = {};
        ASSERT_EQ(driverHandle->createContext(&desc, &hContext), ZE_RESULT_SUCCESS);
        ASSERT_NE(nullptr, hContext);
        context3 = L0::Context::fromHandle(hContext);
        ASSERT_NE(nullptr, context3);
    }

    auto metricContext = device->getMetricContext();
    auto mockMetricContext = reinterpret_cast<MockMetricContext *>(metricContext.get());
    ASSERT_NE(mockMetricContext, nullptr);

    // Activating metric group 1 using context1
    ASSERT_EQ(context1->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    // Confirming that only 1 metric group is activated for metricContext
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1ul);

    // Activating metric group 1 using context2, expect error due to same domain
    ASSERT_EQ(context2->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);

    // Confirming that only 1 metric group is activated for metricContext
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1ul);

    // Attempting to deactivate all metric groups using 3rd context
    ASSERT_EQ(context3->activateMetricGroups(device->toHandle(), 0, nullptr), ZE_RESULT_SUCCESS);

    // Confirming that only 1 metric group is activated for metricContext
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1ul);

    // Deactivating all metric groups associated to 1st context
    ASSERT_EQ(context1->activateMetricGroups(device->toHandle(), 0, nullptr), ZE_RESULT_SUCCESS);

    // Confirming that no metric group is still activated for metricContext
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 0ul);

    // Activating metric group 1 associated to 2nd context
    ASSERT_EQ(context2->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    // Confirming that only 1 metric group is still activated for metricContext
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 1ul);

    // Deactivating all metric groups associated to 2nd context
    ASSERT_EQ(context2->activateMetricGroups(device->toHandle(), 0, nullptr), ZE_RESULT_SUCCESS);

    // Confirming that no metric group is still activated for metricContext
    EXPECT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 0ul);

    ASSERT_EQ(context1->destroy(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(context2->destroy(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(context3->destroy(), ZE_RESULT_SUCCESS);
}

TEST_F(MetricGroupTest, contextCreateQueryPoolReturnsFailureWithoutProperInitializedParams) {
    auto deviceNew = std::make_unique<L0::Device>(nullptr, nullptr);
    ASSERT_NE(deviceNew, nullptr);

    ASSERT_NE(context, nullptr);
    EXPECT_EQ(context->createMetricQueryPool(deviceNew->toHandle(), nullptr, nullptr, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_HANDLE);
    EXPECT_EQ(context->createMetricQueryPool(nullptr, metricGroups[0], nullptr, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_HANDLE);

    queryPoolDesc.count = 1;
    EXPECT_EQ(context->createMetricQueryPool(deviceNew->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    EXPECT_EQ(context->createMetricQueryPool(deviceNew->toHandle(),
                                             metricGroups[0],
                                             nullptr,
                                             &hMetricQueryPool),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    queryPoolDesc.type = ZET_METRIC_QUERY_POOL_TYPE_FORCE_UINT32;
    EXPECT_EQ(context->createMetricQueryPool(deviceNew->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             &hMetricQueryPool),
              ZE_RESULT_ERROR_INVALID_ENUMERATION);
    queryPoolDesc.type = ZET_METRIC_QUERY_POOL_TYPE_EXECUTION;

    queryPoolDesc.count = 0;
    EXPECT_EQ(context->createMetricQueryPool(deviceNew->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             &hMetricQueryPool),
              ZE_RESULT_ERROR_INVALID_SIZE);
    queryPoolDesc.count = 1;

    // Expect failure due to device not initialized properly
    EXPECT_EQ(context->createMetricQueryPool(deviceNew->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             &hMetricQueryPool),
              ZE_RESULT_ERROR_UNINITIALIZED);
}

TEST_F(MetricGroupTest, contextCreateQueryPoolReturnsExpectedResultsWhenMetricGroupIsActivated) {
    // Expect failure due to metric group not being activated
    EXPECT_EQ(context->createMetricQueryPool(device->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             &hMetricQueryPool),
              ZE_RESULT_NOT_READY);

    // Activating metric group 1
    ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(context->createMetricQueryPool(device->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             &hMetricQueryPool),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy(), ZE_RESULT_SUCCESS);
}

TEST_F(MetricGroupTest, createQueryReturnsFailureWithIncorrectParams) {
    // Activating metric group 1
    ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(context->createMetricQueryPool(device->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             &hMetricQueryPool),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(2u, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    zet_metric_query_handle_t hMetricQuery;
    EXPECT_EQ(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(1u, &hMetricQuery),
        ZE_RESULT_ERROR_INVALID_ARGUMENT);
    EXPECT_EQ(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(2u, &hMetricQuery),
        ZE_RESULT_ERROR_INVALID_ARGUMENT);

    // Deactivate metric groups
    ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 0, nullptr), ZE_RESULT_SUCCESS);

    // Expect failure as metric group should be activated
    EXPECT_EQ(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(0u, &hMetricQuery),
        ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE);

    ASSERT_EQ(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy(), ZE_RESULT_SUCCESS);
}

TEST_F(MetricGroupTest, createQueryReturnsExpectedResults) {
    // Activating metric group 1
    ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    queryPoolDesc.count = 2u;
    ASSERT_EQ(context->createMetricQueryPool(device->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             &hMetricQueryPool),
              ZE_RESULT_SUCCESS);

    zet_metric_query_handle_t hMetricQuery;
    EXPECT_EQ(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(0u, &hMetricQuery),
        ZE_RESULT_SUCCESS);

    zet_metric_query_handle_t hMetricQuery2;
    EXPECT_EQ(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(1u, &hMetricQuery2),
        ZE_RESULT_SUCCESS);

    // Creating another query object at the same index should fail.
    EXPECT_EQ(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(0u, &hMetricQuery),
        ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE);

    // Attempting to delete pool without first deleting queries, expect failure
    EXPECT_EQ(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy(),
              ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE);

    EXPECT_EQ(L0::MetricQuery::fromHandle(hMetricQuery)->destroy(), ZE_RESULT_SUCCESS);
    EXPECT_EQ(L0::MetricQuery::fromHandle(hMetricQuery2)->destroy(), ZE_RESULT_SUCCESS);

    // Recycling same index for new query once previous query is deleted
    EXPECT_EQ(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(0u, &hMetricQuery),
        ZE_RESULT_SUCCESS);

    ASSERT_EQ(L0::MetricQuery::fromHandle(hMetricQuery)->destroy(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy(), ZE_RESULT_SUCCESS);
}

TEST_F(MetricGroupTest, queryGetGroupMaskAndDataAddressReturnsExpectedResults) {
    // Activating metric group
    ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    queryPoolDesc.count = 1u;
    ASSERT_EQ(context->createMetricQueryPool(device->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             &hMetricQueryPool),
              ZE_RESULT_SUCCESS);

    zet_metric_query_handle_t hMetricQuery;
    EXPECT_EQ(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(0u, &hMetricQuery),
        ZE_RESULT_SUCCESS);

    ASSERT_NE(hMetricQuery, nullptr);
    auto metricQuery = L0::MetricQuery::fromHandle(hMetricQuery);

    EXPECT_EQ(metricQuery->getMetricGroupMask(), 0b1000);
    EXPECT_EQ(reinterpret_cast<uint64_t *>(metricQuery->getMetricAddrPtr())[0], 0u);
    EXPECT_NE(reinterpret_cast<uint64_t *>(metricQuery->getMetricAddrPtr())[3], 0u);
    ASSERT_EQ(L0::MetricQuery::fromHandle(hMetricQuery)->destroy(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy(), ZE_RESULT_SUCCESS);
}

TEST_F(MetricGroupTest, queryGetDataAndResetReturnsExpectedResults) {
    // Activating metric group 1
    ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    queryPoolDesc.count = 1u;
    ASSERT_EQ(context->createMetricQueryPool(device->toHandle(),
                                             metricGroups[0],
                                             &queryPoolDesc,
                                             &hMetricQueryPool),
              ZE_RESULT_SUCCESS);

    zet_metric_query_handle_t hMetricQuery;
    ASSERT_EQ(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(0u, &hMetricQuery),
        ZE_RESULT_SUCCESS);

    ASSERT_NE(hMetricQuery, nullptr);
    auto metricQuery = L0::MetricQuery::fromHandle(hMetricQuery);

    // Retrieve size and values of query data
    size_t rawDataSize = 0u;
    std::vector<uint64_t> rawData = {};
    EXPECT_EQ(metricQuery->getData(&rawDataSize, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_GT(rawDataSize, 0u);

    size_t rawDataCount = rawDataSize / sizeof(uint64_t);
    rawData.resize(rawDataCount);
    EXPECT_EQ(metricQuery->getData(&rawDataSize, reinterpret_cast<uint8_t *>(rawData.data())),
              ZE_RESULT_SUCCESS);

    // Expect 0 values for unused metric query
    for (uint8_t i = 0; i < rawDataCount; i++) {
        EXPECT_EQ(rawData[i], 0u);
    }

    // Injecting dummy data into pMetricQuery (simulating post execution step)
    auto mockMetricQuery = reinterpret_cast<MockMetricQuery *>(metricQuery);
    mockMetricQuery->injectDummyData();
    EXPECT_EQ(mockMetricQuery->getData(&rawDataSize, reinterpret_cast<uint8_t *>(rawData.data())),
              ZE_RESULT_SUCCESS);

    // Expect counter values to be more than 0
    for (uint8_t i = 0; i < rawDataCount; i++) {
        EXPECT_GT(rawData[i], 0u);
    }

    // Resetting metric query and retrieving data
    EXPECT_EQ(metricQuery->reset(), ZE_RESULT_SUCCESS);
    EXPECT_EQ(metricQuery->getData(&rawDataSize, reinterpret_cast<uint8_t *>(rawData.data())),
              ZE_RESULT_SUCCESS);

    // Expect 0 values for metric query that has been reset
    for (uint8_t i = 0; i < rawDataCount; i++) {
        EXPECT_EQ(rawData[i], 0u);
    }

    ASSERT_EQ(L0::MetricQuery::fromHandle(hMetricQuery)->destroy(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy(), ZE_RESULT_SUCCESS);
}

struct MetricGroupCalculateTest : public Test<MetricGroupShared> {
    void SetUp() override {
        MetricGroupShared::SetUp();

        ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
                  ZE_RESULT_SUCCESS);

        queryPoolDesc.count = 1u;
        ASSERT_EQ(context->createMetricQueryPool(device->toHandle(),
                                                 metricGroups[0],
                                                 &queryPoolDesc,
                                                 &hMetricQueryPool),
                  ZE_RESULT_SUCCESS);

        ASSERT_EQ(
            L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(0u, &hMetricQuery),
            ZE_RESULT_SUCCESS);

        ASSERT_NE(hMetricQuery, nullptr);
        auto metricQuery = L0::MetricQuery::fromHandle(hMetricQuery);

        // Retrieve size and values of query data
        EXPECT_EQ(metricQuery->getData(&rawDataSize, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_GT(rawDataSize, 0u);

        size_t rawDataCount = rawDataSize / sizeof(uint64_t);
        rawData.resize(rawDataCount);

        auto mockMetricQuery = reinterpret_cast<MockMetricQuery *>(metricQuery);
        mockMetricQuery->injectDummyData();
        EXPECT_EQ(
            mockMetricQuery->getData(&rawDataSize, reinterpret_cast<uint8_t *>(rawData.data())),
            ZE_RESULT_SUCCESS);
    }

    void TearDown() override {
        ASSERT_EQ(L0::MetricQuery::fromHandle(hMetricQuery)->destroy(), ZE_RESULT_SUCCESS);
        ASSERT_EQ(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy(), ZE_RESULT_SUCCESS);

        MetricGroupShared::TearDown();
    }

    zet_metric_query_pool_handle_t hMetricQueryPool = nullptr;
    zet_metric_query_handle_t hMetricQuery = nullptr;
    std::vector<uint64_t> rawData = {};
    size_t rawDataSize = 0u;
};

TEST_F(MetricGroupCalculateTest, calculateMetricValuesReturnsFailureWithIncorrectInput) {
    uint32_t metricValueCount = 0;
    // Expect error when rawData is nullptr
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])
                  ->calculateMetricValues(ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                          rawDataSize,
                                          nullptr,
                                          &metricValueCount,
                                          nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    // Expect error when pMetricValueCount is nullptr
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])
                  ->calculateMetricValues(ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                          rawDataSize,
                                          reinterpret_cast<uint8_t *>(rawData.data()),
                                          nullptr,
                                          nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    // Expect error when metric group calculation type exceeding max
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])
                  ->calculateMetricValues(ZET_METRIC_GROUP_CALCULATION_TYPE_FORCE_UINT32,
                                          rawDataSize,
                                          reinterpret_cast<uint8_t *>(rawData.data()),
                                          &metricValueCount,
                                          nullptr),
              ZE_RESULT_ERROR_INVALID_ENUMERATION);

    // Expect success when calculating maxim metric values (pMetricValueCount is equal to metric
    // size)
    metricValueCount = 1;
    std::vector<zet_typed_value_t> metricValues(metricValueCount);
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])
                  ->calculateMetricValues(ZET_METRIC_GROUP_CALCULATION_TYPE_MAX_METRIC_VALUES,
                                          rawDataSize,
                                          reinterpret_cast<uint8_t *>(rawData.data()),
                                          &metricValueCount,
                                          metricValues.data()),
              ZE_RESULT_SUCCESS);
}

TEST_F(MetricGroupCalculateTest, calculateMetricValuesReturnsExpectedResults) {
    uint32_t metricValueCount = 0;
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])
                  ->calculateMetricValues(ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                          rawDataSize,
                                          reinterpret_cast<uint8_t *>(rawData.data()),
                                          &metricValueCount,
                                          nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_GT(metricValueCount, 0);

    std::vector<zet_typed_value_t> metricValues(metricValueCount);
    EXPECT_EQ(L0::MetricGroup::fromHandle(metricGroups[0])
                  ->calculateMetricValues(ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                          rawDataSize,
                                          reinterpret_cast<uint8_t *>(rawData.data()),
                                          &metricValueCount,
                                          metricValues.data()),
              ZE_RESULT_SUCCESS);

    for (const auto &value : metricValues) {
        // All counters return UINT64 values
        EXPECT_EQ(value.type, ZET_VALUE_TYPE_UINT64);

        // Workload is run so expect all counters to be more than 0
        EXPECT_GT(value.value.ui64, 0llu);
    }
}

struct MultiDeviceMetricTest : public Test<MultiDeviceFixture> {
    ze_context_handle_t hContext = nullptr;
};

TEST_F(MultiDeviceMetricTest, activatingMetricGroupsNotAssociatedWithDeviceReturnsFailure) {
    ASSERT_NE(driverHandle, nullptr);

    uint32_t count = 0;
    std::vector<ze_device_handle_t> phDevices;
    // Retrieving multiple devices
    {
        ASSERT_EQ(driverHandle->getDevice(&count, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_GT(count, 1u);
        phDevices.resize(count);
        ASSERT_EQ(driverHandle->getDevice(&count, phDevices.data()), ZE_RESULT_SUCCESS);
    }

    L0::Context *context = nullptr;
    // Creating context
    {
        ze_context_desc_t desc;
        ASSERT_EQ(driverHandle->createContext(&desc, &hContext), ZE_RESULT_SUCCESS);
        ASSERT_NE(nullptr, hContext);
        context = L0::Context::fromHandle(hContext);
        ASSERT_NE(nullptr, context);
    }

    auto device1 = L0::Device::fromHandle(phDevices[0]);
    auto metricContext = device1->getMetricContext();
    auto mockMetricContext = reinterpret_cast<MockMetricContext *>(metricContext.get());

    std::vector<zet_metric_group_handle_t> metricGroups;
    uint32_t metricGroupCount = 0u;
    // Retreiving metric groups for second device
    {
        auto device2 = L0::Device::fromHandle(phDevices[1]);

        ASSERT_EQ(device2->metricGroupGet(&metricGroupCount, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_GT(metricGroupCount, 0u);
        metricGroups.resize(metricGroupCount);
        ASSERT_EQ(device2->metricGroupGet(&metricGroupCount, metricGroups.data()),
                  ZE_RESULT_SUCCESS);
    }

    // Activating metric groups not associated with device, expect failure
    ASSERT_EQ(
        context->activateMetricGroups(device1->toHandle(), metricGroupCount, metricGroups.data()),
        ZE_RESULT_ERROR_INVALID_ARGUMENT);

    // Confirming that no metric groups are activated
    ASSERT_EQ(mockMetricContext->getActivatedMetricGroupsSize(), 0ul);

    ASSERT_EQ(context->destroy(), ZE_RESULT_SUCCESS);
}

} // namespace ult
} // namespace L0
