/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

struct Event : public UmdTest {
    void SetUp() override {
        UmdTest::SetUp();

        const ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                                    nullptr,
                                                    ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                                    Event::eventPoolCap};
        ze_result_t ret;
        scopedEventPool = zeScope::eventPoolCreate(zeContext, eventPoolDesc, 1, zeDevice, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        eventPool = scopedEventPool.get();
    }

    void TearDown() override { UmdTest::TearDown(); }

    ze_event_pool_handle_t eventPool = nullptr;
    // Size of event pool.
    static const uint32_t eventPoolCap = 3;

  private:
    zeScope::SharedPtr<ze_event_pool_handle_t> scopedEventPool = nullptr;
};

TEST_F(Event, CreateEventWithInvalidParameters) {
    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST,
                                 ZE_EVENT_SCOPE_FLAG_HOST};
    ze_event_handle_t event = nullptr;

    EXPECT_EQ(zeEventCreate(eventPool, nullptr, &event), ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    EXPECT_EQ(zeEventCreate(eventPool, &eventDesc, nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    EXPECT_EQ(zeEventCreate(eventPool, nullptr, nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    eventDesc.signal = 0x08;
    EXPECT_EQ(zeEventCreate(eventPool, &eventDesc, &event), ZE_RESULT_ERROR_INVALID_ENUMERATION);

    eventDesc.signal = ZE_EVENT_SCOPE_FLAG_HOST;
    eventDesc.wait = 0x08;
    EXPECT_EQ(zeEventCreate(eventPool, &eventDesc, &event), ZE_RESULT_ERROR_INVALID_ENUMERATION);
}

TEST_F(Event, AllocateEventTwiceExpectError) {
    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST,
                                 ZE_EVENT_SCOPE_FLAG_HOST};
    ze_result_t ret;

    auto scopedEvent = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

    auto expectNullptr = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_ERROR_INVALID_ARGUMENT);
    EXPECT_EQ(expectNullptr, nullptr);
}

TEST_F(Event, CreateEventShouldBeSuccessful) {
    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST,
                                 ZE_EVENT_SCOPE_FLAG_HOST};
    ze_result_t ret;

    // Event index 0.
    auto scopedEvent0 = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

    eventDesc.index = 1;
    auto scopedEvent1 = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

    // Event index 2.
    eventDesc.index = 2;
    auto scopedEvent2 = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
}
