/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <ze_api.h>
#include <ze_ddi.h>
#include <zer_ddi.h>

namespace L0 {
extern ze_global_dditable_t zeGlobalDdiTable;
extern ze_driver_dditable_t zeDriverDdiTable;
extern ze_device_dditable_t zeDeviceDdiTable;
extern ze_context_dditable_t zeContextDdiTable;
extern ze_command_queue_dditable_t zeCommandQueueDdiTable;
extern ze_command_list_dditable_t zeCommandListDdiTable;
extern ze_command_list_exp_dditable_t zeCommandListExpDdiTable;
extern ze_mem_dditable_t zeMemDdiTable;
extern ze_fence_dditable_t zeFenceDdiTable;
extern ze_event_pool_dditable_t zeEventPoolDdiTable;
extern ze_event_dditable_t zeEventDdiTable;
extern ze_physical_mem_dditable_t zePhysicalMemDdiTable;
extern ze_virtual_mem_dditable_t zeVirtualMemDdiTable;
} // namespace L0

ze_dditable_driver_t coreDDITable = {
    .version = ZE_API_VERSION_CURRENT,
    .isValidFlag = 1,
    .RTASBuilder = nullptr,
    .RTASBuilderExp = nullptr,
    .RTASParallelOperation = nullptr,
    .RTASParallelOperationExp = nullptr,
    .Global = &L0::zeGlobalDdiTable,
    .Driver = &L0::zeDriverDdiTable,
    .DriverExp = nullptr,
    .Device = &L0::zeDeviceDdiTable,
    .DeviceExp = nullptr,
    .Context = &L0::zeContextDdiTable,
    .CommandQueue = &L0::zeCommandQueueDdiTable,
    .CommandList = &L0::zeCommandListDdiTable,
    .CommandListExp = &L0::zeCommandListExpDdiTable,
    .Image = nullptr,
    .ImageExp = nullptr,
    .Mem = &L0::zeMemDdiTable,
    .MemExp = nullptr,
    .Fence = &L0::zeFenceDdiTable,
    .EventPool = &L0::zeEventPoolDdiTable,
    .Event = &L0::zeEventDdiTable,
    .EventExp = nullptr,
    .Module = nullptr,
    .ModuleBuildLog = nullptr,
    .Kernel = nullptr,
    .KernelExp = nullptr,
    .Sampler = nullptr,
    .PhysicalMem = &L0::zePhysicalMemDdiTable,
    .VirtualMem = &L0::zeVirtualMemDdiTable,
    .FabricVertexExp = nullptr,
    .FabricEdgeExp = nullptr,
};

zer_dditable_driver_t runtimeDdiTable = {};
