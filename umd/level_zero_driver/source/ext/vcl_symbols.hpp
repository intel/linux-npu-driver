/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "npu_driver_compiler.h"
#include "vpu_driver/source/utilities/log.hpp"

#include <array>
#include <dlfcn.h>
#include <memory>
#include <string>

// TODO: Macros with VCL_COMPILER_VERSION_MAJOR are tempoarilly added and can be removed after
// compiler change is merged:
// https://github.com/intel-innersource/applications.ai.vpu-accelerators.vpux-plugin/pull/13853
#if VCL_COMPILER_VERSION_MAJOR == 7
#define VCL_COMPILER_VERSION_MAJOR_LEGACY 6
#define VCL_COMPILER_VERSION_MAJOR_NEXT VCL_COMPILER_VERSION_MAJOR

typedef enum __vcl_platform_t {
    VCL_PLATFORM_UNKNOWN = -1,
    VCL_PLATFORM_VPU3700 = 0,
    VCL_PLATFORM_VPU3720 = 1,
    VCL_PLATFORM_VPU4000 = 2,
} vcl_platform_t;

typedef struct __vcl_compiler_desc_legacy_t {
    vcl_platform_t platform;
    vcl_log_level_t debug_level;
} vcl_compiler_desc_legacy_t;

using vcl_compiler_desc_next_t = vcl_compiler_desc_t;
#else
#define VCL_COMPILER_VERSION_MAJOR_LEGACY VCL_COMPILER_VERSION_MAJOR
#define VCL_COMPILER_VERSION_MAJOR_NEXT 7

typedef struct __vcl_device_desc_t {
    uint64_t size;
    uint32_t deviceID;
    uint16_t revision;
    uint32_t tileCount;
} vcl_device_desc_t;

typedef struct __vcl_compiler_desc_next_t {
    vcl_version_info_t version;
    vcl_log_level_t debugLevel;
} vcl_compiler_desc_next_t;

using vcl_compiler_desc_legacy_t = vcl_compiler_desc_t;

vcl_result_t vclGetVersion(vcl_version_info_t *compilerVersion,
                           vcl_version_info_t *profilingVersion);
#endif

vcl_result_t vclCompilerCreateLegacy(vcl_compiler_desc_legacy_t desc,
                                     vcl_compiler_handle_t *compiler,
                                     vcl_log_handle_t *logHandle);

vcl_result_t vclCompilerCreateNext(vcl_compiler_desc_next_t *compilerDesc,
                                   vcl_device_desc_t *deviceDesc,
                                   vcl_compiler_handle_t *compiler,
                                   vcl_log_handle_t *logHandle);

class Vcl {
  public:
    static Vcl &sym() {
        static Vcl vclSym;
        return vclSym;
    }

    bool ok() { return handle != nullptr; }

  private:
    template <typename... Arg>
    static vcl_result_t missingSymbol(Arg... args) {
        LOG_W("Missing symbol function");
        return VCL_RESULT_ERROR_UNKNOWN;
    }

    template <typename T>
    T getSymbolAddr(const char *name) {
        void *sym = dlsym(handle.get(), name);
        if (!sym) {
            LOG_W("Failed to load %s symbol, error: %s", name, dlerror());
            return &missingSymbol;
        }
        return reinterpret_cast<T>(sym);
    }

    Vcl() {
        std::string errorMsg;
        for (auto name : compilerNames) {
            handle = VclHandle(dlopen(name, RTLD_LAZY | RTLD_LOCAL), &closeHandle);
            if (handle)
                break;

            errorMsg += std::string("\ndlopen: ") + name + ", error: " + dlerror();
        }

        if (!handle) {
            LOG_E("Failed to load driver compiler:%s", errorMsg.c_str());
            return;
        }

        compilerCreate = getSymbolAddr<decltype(compilerCreate)>("vclCompilerCreate");
        compilerCreateLegacy = getSymbolAddr<decltype(compilerCreateLegacy)>("vclCompilerCreate");
        compilerDestroy = getSymbolAddr<decltype(compilerDestroy)>("vclCompilerDestroy");
        compilerGetProperties =
            getSymbolAddr<decltype(compilerGetProperties)>("vclCompilerGetProperties");
        queryNetworkCreate = getSymbolAddr<decltype(queryNetworkCreate)>("vclQueryNetworkCreate");
        queryNetworkDestroy =
            getSymbolAddr<decltype(queryNetworkDestroy)>("vclQueryNetworkDestroy");
        queryNetwork = getSymbolAddr<decltype(queryNetwork)>("vclQueryNetwork");
        executableCreate = getSymbolAddr<decltype(executableCreate)>("vclExecutableCreate");
        executableDestroy = getSymbolAddr<decltype(executableDestroy)>("vclExecutableDestroy");
        executableGetSerializableBlob = getSymbolAddr<decltype(executableGetSerializableBlob)>(
            "vclExecutableGetSerializableBlob");
        profilingCreate = getSymbolAddr<decltype(profilingCreate)>("vclProfilingCreate");
        profilingDestroy = getSymbolAddr<decltype(profilingDestroy)>("vclProfilingDestroy");
        profilingGetProperties =
            getSymbolAddr<decltype(profilingGetProperties)>("vclProfilingGetProperties");
        getDecodedProfilingBuffer =
            getSymbolAddr<decltype(getDecodedProfilingBuffer)>("vclGetDecodedProfilingBuffer");
        logHandleGetString = getSymbolAddr<decltype(logHandleGetString)>("vclLogHandleGetString");
        allocatedExecutableCreate =
            getSymbolAddr<decltype(allocatedExecutableCreate)>("vclAllocatedExecutableCreate");
        getVersion = getSymbolAddr<decltype(getVersion)>("vclGetVersion");
    }

    static void closeHandle(void *handle) noexcept { dlclose(handle); }

  public:
    decltype(vclCompilerCreateNext) *compilerCreate = &missingSymbol;
    decltype(vclCompilerCreateLegacy) *compilerCreateLegacy = &missingSymbol;
    decltype(vclCompilerDestroy) *compilerDestroy = &missingSymbol;
    decltype(vclCompilerGetProperties) *compilerGetProperties = &missingSymbol;
    decltype(vclQueryNetworkCreate) *queryNetworkCreate = &missingSymbol;
    decltype(vclQueryNetworkDestroy) *queryNetworkDestroy = &missingSymbol;
    decltype(vclQueryNetwork) *queryNetwork = &missingSymbol;
    decltype(vclExecutableCreate) *executableCreate = &missingSymbol;
    decltype(vclExecutableDestroy) *executableDestroy = &missingSymbol;
    decltype(vclExecutableGetSerializableBlob) *executableGetSerializableBlob = &missingSymbol;
    decltype(vclProfilingCreate) *profilingCreate = &missingSymbol;
    decltype(vclProfilingDestroy) *profilingDestroy = &missingSymbol;
    decltype(vclProfilingGetProperties) *profilingGetProperties = &missingSymbol;
    decltype(vclGetDecodedProfilingBuffer) *getDecodedProfilingBuffer = &missingSymbol;
    decltype(vclLogHandleGetString) *logHandleGetString = &missingSymbol;
    decltype(vclAllocatedExecutableCreate) *allocatedExecutableCreate = &missingSymbol;
    decltype(vclGetVersion) *getVersion = &missingSymbol;

  private:
    using VclHandle = std::unique_ptr<void, decltype(&closeHandle)>;
    VclHandle handle = VclHandle(nullptr, nullptr);

    std::array<const char *, 2> compilerNames = {"libnpu_driver_compiler.so",
                                                 "libvpux_driver_compiler.so"};
};
