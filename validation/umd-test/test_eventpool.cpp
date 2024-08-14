/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

struct EventPool : public UmdTest {};

TEST_F(EventPool, CreateEventPoolWithInvalidParameters) {
    ze_event_pool_handle_t eventPool = nullptr;
    ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                          nullptr,
                                          ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                          1};

    EXPECT_EQ(zeEventPoolCreate(zeContext, nullptr, 1, &zeDevice, &eventPool),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    EXPECT_EQ(zeEventPoolCreate(zeContext, &eventPoolDesc, 1, &zeDevice, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    EXPECT_EQ(zeEventPoolCreate(zeContext, nullptr, 1, &zeDevice, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    eventPoolDesc.count = 0;
    EXPECT_EQ(zeEventPoolCreate(zeContext, &eventPoolDesc, 1, &zeDevice, &eventPool),
              ZE_RESULT_ERROR_INVALID_SIZE);
    eventPoolDesc.count = 1;
}

TEST_F(EventPool, CreateEventPoolShouldBeSuccessful) {
    ze_event_pool_handle_t eventPool = nullptr;
    ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                          nullptr,
                                          ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                          1};

    EXPECT_EQ(zeEventPoolCreate(zeContext, &eventPoolDesc, 1, &zeDevice, &eventPool),
              ZE_RESULT_SUCCESS);
    EXPECT_NE(eventPool, nullptr);
    EXPECT_EQ(zeEventPoolDestroy(eventPool), ZE_RESULT_SUCCESS);
}
