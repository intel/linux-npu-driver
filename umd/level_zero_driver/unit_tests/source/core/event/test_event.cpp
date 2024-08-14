/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "gtest/gtest.h"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/core/source/event/eventpool.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <level_zero/ze_api.h>
#include <vector>

namespace L0 {
namespace ult {

struct EventPoolTest : public Test<CommandQueueFixture> {
    // Default event pool descriptor.
    ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                          nullptr,
                                          ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                          1};

    ze_event_pool_handle_t hEventPool = nullptr;
};

TEST_F(EventPoolTest, givenCallCreateEventPoolReturnsSuccess) {
    ASSERT_NE(nullptr, context);

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeEventPoolCreate(context, &eventPoolDesc, 0, nullptr, &hEventPool));
    EXPECT_NE(nullptr, hEventPool);

    // Deallocate the event pool.
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventPoolDestroy(hEventPool));
}

TEST_F(EventPoolTest, eventPoolCreateHandleErrors) {
    ASSERT_NE(nullptr, context);

    // ZE_RESULT_ERROR_INVALID_NULL_POINTER if desc == nullptr or phEventPool.
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              zeEventPoolCreate(context, nullptr, 0, nullptr, &hEventPool));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              zeEventPoolCreate(context, &eventPoolDesc, 0, nullptr, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              zeEventPoolCreate(context, nullptr, 0, nullptr, nullptr));

    eventPoolDesc.count = 0;
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_SIZE,
              zeEventPoolCreate(context, &eventPoolDesc, 0, nullptr, &hEventPool));
    eventPoolDesc.count = 1;

    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_SIZE,
              zeEventPoolCreate(context, &eventPoolDesc, 1, nullptr, &hEventPool));
}

TEST_F(EventPoolTest, eventPoolCreateRetunsSuccessForManyEvents) {
    // 10 events allocatable event pool.
    const uint32_t nEvents = 10;
    eventPoolDesc.count = nEvents;
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeEventPoolCreate(context, &eventPoolDesc, 0, nullptr, &hEventPool));

    // Deallocate the event pool.
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventPoolDestroy(hEventPool));
}

struct EventTest : public EventPoolTest {
    void SetUp() override {
        EventPoolTest::SetUp();

        eventPoolDesc.count = 1;

        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeEventPoolCreate(context, &eventPoolDesc, 0, nullptr, &hEventPool));

        pEvPool = EventPool::fromHandle(hEventPool);
        ASSERT_EQ(ZE_RESULT_SUCCESS, pEvPool->createEvent(&eventDesc, &hEvent));
    }

    void TearDown() override {
        if (hEvent != nullptr) {
            ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hEvent));
        }

        if (hEventPool != nullptr) {
            ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventPoolDestroy(hEventPool));
            hEventPool = nullptr;
        }

        EventPoolTest::TearDown();
    }

    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST};
    ze_event_handle_t hEvent = nullptr;
    EventPool *pEvPool = nullptr;
};

TEST_F(EventTest, eventStatusChangesAndQuery) {
    // Report errors on wrong handles.
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, zeEventQueryStatus(nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, zeEventHostSignal(nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, zeEventHostReset(nullptr));

    // Initially the status query returns NOT_READY.
    auto event = Event::fromHandle(hEvent);
    ASSERT_NE(nullptr, event);
    EXPECT_EQ(ZE_RESULT_NOT_READY, event->queryStatus());

    // Signaling the event from host should change the status.
    // Multiple calling shouldn't impact the result.
    EXPECT_EQ(ZE_RESULT_SUCCESS, event->hostSignal());
    EXPECT_EQ(ZE_RESULT_SUCCESS, event->queryStatus());
    EXPECT_EQ(ZE_RESULT_SUCCESS, event->hostSignal());
    EXPECT_EQ(ZE_RESULT_SUCCESS, event->queryStatus());

    // Resetting the event fro host should change the status.
    // Multiple calling shouldn't impact the result.
    EXPECT_EQ(ZE_RESULT_SUCCESS, event->reset());
    EXPECT_EQ(ZE_RESULT_NOT_READY, event->queryStatus());
    EXPECT_EQ(ZE_RESULT_SUCCESS, event->reset());
    EXPECT_EQ(ZE_RESULT_NOT_READY, event->queryStatus());
}

TEST_F(EventTest, givenCallHostSyncReturnsSignalStatus) {
    auto ev = Event::fromHandle(hEvent);
    ASSERT_NE(nullptr, ev);

    // The event expected to be initialized.
    EXPECT_EQ(ZE_RESULT_NOT_READY, ev->hostSynchronize(0u));
}

TEST_F(EventTest, eventCreateHandleErrors) {
    auto evPool = EventPool::fromHandle(hEventPool);
    ASSERT_NE(nullptr, evPool);

    ze_event_handle_t hTestEvent = nullptr;
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, evPool->createEvent(nullptr, &hTestEvent));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, evPool->createEvent(&eventDesc, nullptr));
}

struct MultipleEventTest : public EventPoolTest {
    void SetUp() override { EventPoolTest::SetUp(); }

    void TearDown() override {
        if (hEventPool != nullptr) {
            ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventPoolDestroy(hEventPool));
            hEventPool = nullptr;
        }

        EventPoolTest::TearDown();
    }

    void prepareEventPool(uint32_t numEvents) {
        eventPoolDesc.count = numEvents;
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeEventPoolCreate(context, &eventPoolDesc, 0, nullptr, &hEventPool));
    }

    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST};
    EventPool *pEvPool = nullptr;
    ze_event_handle_t hTestEvent0 = nullptr;
    ze_event_handle_t hTestEvent1 = nullptr;
    ze_event_handle_t hTestEvent2 = nullptr;
};

TEST_F(MultipleEventTest, eventPoolAllocatesMultipleEvents) {
    // Maximum 3 allocation capability event pool.
    const uint32_t nMaxEvents = 3;
    prepareEventPool(nMaxEvents);
    auto evPool = EventPool::fromHandle(hEventPool);
    ASSERT_NE(nullptr, evPool);

    // Index 0 in event pool.
    eventDesc.index = 0;
    EXPECT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent0));
    EXPECT_NE(nullptr, hTestEvent0);

    // Index 1 in event pool.
    eventDesc.index = 1;
    EXPECT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent1));
    EXPECT_NE(nullptr, hTestEvent1);

    eventDesc.index = 2;
    EXPECT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent2));
    EXPECT_NE(nullptr, hTestEvent2);

    // Allocated events should be different each other.
    EXPECT_NE(hTestEvent0, hTestEvent1);
    EXPECT_NE(hTestEvent1, hTestEvent2);
    EXPECT_NE(hTestEvent0, hTestEvent1);

    // Events should hold properly offsetted pointers.
    auto ev0 = Event::fromHandle(hTestEvent0);
    auto ev1 = Event::fromHandle(hTestEvent1);
    auto ev2 = Event::fromHandle(hTestEvent2);
    ASSERT_NE(nullptr, ev0);
    ASSERT_NE(nullptr, ev1);
    ASSERT_NE(nullptr, ev2);

    // Each pointer expected to sizeof(KMDEventDataType) apart at least.
    auto evPtr0 = reinterpret_cast<const void *>(ev0->getSyncPointer());
    auto evPtr1 = reinterpret_cast<const void *>(ev1->getSyncPointer());
    auto evPtr2 = reinterpret_cast<const void *>(ev2->getSyncPointer());
    EXPECT_EQ(sizeof(VPU::VPUEventCommand::JsmEventData),
              reinterpret_cast<size_t>(static_cast<const uint8_t *>(evPtr1)) -
                  reinterpret_cast<size_t>(static_cast<const uint8_t *>(evPtr0)));
    EXPECT_EQ(sizeof(VPU::VPUEventCommand::JsmEventData),
              reinterpret_cast<size_t>(static_cast<const uint8_t *>(evPtr2)) -
                  reinterpret_cast<size_t>(static_cast<const uint8_t *>(evPtr1)));

    // Deallocate
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent0));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent1));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent2));
}

TEST_F(MultipleEventTest, eventPoolReAllocatesMultipleEvents) {
    // Maximum 2 allocation capability event pool.
    const uint32_t nMaxEvents = 2;
    prepareEventPool(nMaxEvents);
    auto evPool = EventPool::fromHandle(hEventPool);
    ASSERT_NE(nullptr, evPool);

    // Index 0 in event pool.
    eventDesc.index = 0;
    ASSERT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent0));
    ASSERT_NE(nullptr, hTestEvent0);

    // Reallocation attempt without destroy should be failed.
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT, evPool->createEvent(&eventDesc, &hTestEvent0));

    // Destory & re-allocate.
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent0));
    EXPECT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent0));
    EXPECT_NE(nullptr, hTestEvent0);

    // Index 1 in event pool.
    eventDesc.index = 1;
    ASSERT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent1));
    ASSERT_NE(nullptr, hTestEvent1);

    // Reallocation attempt without destroy should be failed.
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT, evPool->createEvent(&eventDesc, &hTestEvent1));

    // Destory & re-allocate.
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent1));
    EXPECT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent1));
    EXPECT_NE(nullptr, hTestEvent1);

    // Resource deallocation.
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent0));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent1));
}

TEST_F(MultipleEventTest, eventPoolHandleAllocationErrors) {
    // Maximum 2 allocation capability event pool.
    const uint32_t nMaxEvents = 2;
    prepareEventPool(nMaxEvents);
    auto evPool = EventPool::fromHandle(hEventPool);

    // Index 0 in event pool.
    eventDesc.index = 0;
    EXPECT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent0));
    EXPECT_NE(nullptr, hTestEvent0);

    // Index 0 has already been allocated.
    eventDesc.index = 0;
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT, evPool->createEvent(&eventDesc, &hTestEvent1));
    EXPECT_EQ(nullptr, hTestEvent1);

    // Index 2 is out of bound.
    eventDesc.index = 2;
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT, evPool->createEvent(&eventDesc, &hTestEvent1));
    EXPECT_EQ(nullptr, hTestEvent1);

    // Deallocating index 0 will free the space.
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent0));
    eventDesc.index = 0;
    EXPECT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent0));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent0));
}

TEST_F(MultipleEventTest, memoryPinningHappensOnlyOnceOnMultipleEventCreations) {
    // A maximum 5 allocation capability event pool.
    const uint32_t nMaxEvents = 5;
    prepareEventPool(nMaxEvents);
    auto evPool = EventPool::fromHandle(hEventPool);

    // Allocate events.
    std::vector<ze_event_handle_t> evHandles;
    for (uint32_t i = 0; i < nMaxEvents; ++i) {
        hTestEvent0 = nullptr;
        eventDesc.index = i;
        ASSERT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&eventDesc, &hTestEvent0));
        ASSERT_NE(nullptr, hTestEvent0);
        evHandles.push_back(hTestEvent0);
    }

    // Memory allocation should happened once.
    EXPECT_EQ(1u, osInfc.callCntAlloc);

    // Deallocate.
    for (auto &hTestEvent0 : evHandles) {
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hTestEvent0));
    }
}

} // namespace ult
} // namespace L0
