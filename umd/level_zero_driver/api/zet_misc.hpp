/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "ze_ddi_common.h"

#include <loader/ze_loader.h>
#include <string>
#include <ze_api.h>
#include <ze_ddi.h>

#pragma once

extern zet_dditable_driver_t toolsDdiTable;
extern ze_dditable_driver_t coreDDITable;
extern zes_dditable_driver_t sysmanDdiTable;
extern zer_dditable_driver_t runtimeDdiTable;

namespace L0 {
struct _ze_generic_handle_t : public ze_handle_t {
    _ze_generic_handle_t() {
        pCore = &coreDDITable;
        pTools = &toolsDdiTable;
        pSysman = &sysmanDdiTable;
        pRuntime = &runtimeDdiTable;
    };

    inline ze_handle_t *toHandle() { return this; }
};

void *getLoaderHandle();
std::string getLoaderVersion();

ze_result_t translateHandle(zel_handle_type_t type, void *handler, void **pHandler);

template <class T>
static inline ze_result_t translateHandle(zel_handle_type_t type, T handler, T *pHandler) {
    return translateHandle(type, handler, reinterpret_cast<void **>(pHandler));
}

template <class T>
static inline ze_result_t translateHandle(zel_handle_type_t type, T &handler) {
    return translateHandle(type, handler, &handler);
}
} // namespace L0
