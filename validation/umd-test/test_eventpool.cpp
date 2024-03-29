/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

struct EventPool : public UmdTest {};

// TODO: Validation layer is disabled. OpenVino issue: EISW-113275
TEST_F(EventPool, DISABLED_CreateEventPoolWithInvalidParameters) {
    ze_event_pool_handle_t eventPool = nullptr;
    ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                          nullptr,
                                          ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                          1};

    EXPECT_EQ(zeEventPoolCreate(nullptr, &eventPoolDesc, 1, &zeDevice, &eventPool),
              ZE_RESULT_ERROR_INVALID_NULL_HANDLE);
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
    EXPECT_EQ(zeEventPoolCreate(zeContext, &eventPoolDesc, 1, nullptr, &eventPool),
              ZE_RESULT_ERROR_INVALID_SIZE);
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

// TODO: Validation layer is disabled. OpenVino issue: EISW-113275
TEST_F(EventPool, DISABLED_DestroyEventPoolWithInvalidParameter) {
    EXPECT_EQ(zeEventPoolDestroy(nullptr), ZE_RESULT_ERROR_INVALID_NULL_HANDLE);
}
