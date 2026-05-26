/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "npu_driver_compiler.h"
#include "vpu_driver/source/utilities/log.hpp"

#include <dlfcn.h>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class Vcl {
  public:
    static Vcl &sym() {
        static Vcl vclSym;
        return vclSym;
    }

    bool ok() { return handle != nullptr; }

    std::string getLibPath() const {
        if (!handle)
            return {};

        Dl_info info = {};
        int ret = dladdr(reinterpret_cast<void *>(getVersion), &info);
        if (ret == 0)
            return {};
        return info.dli_fname;
    }

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

    template <typename T>
    T getSymbolAddrOrNullptr(const char *name) {
        void *sym = dlsym(handle.get(), name);
        if (!sym) {
            LOG_W("Failed to load %s symbol, error: %s", name, dlerror());
            return nullptr;
        }
        return reinterpret_cast<T>(sym);
    }

    static std::filesystem::path getDriverLibDir() {
#ifdef NPU_ALT_DEPENDENCY_PATH_OVERRIDE
        if (getenv("NPU_ALT_DEPENDENCY_PATH"))
            return std::filesystem::path(getenv("NPU_ALT_DEPENDENCY_PATH"));
#endif

        Dl_info info = {};
        int ret = dladdr(reinterpret_cast<void *>(getDriverLibDir), &info);
        if (ret == 0) {
            LOG_E("Failed to get driver library path");
            return {};
        }
        return std::filesystem::path(info.dli_fname).parent_path();
    }

    Vcl() {
        // First try to load library placed in same directory as driver
        std::vector<std::string> libToLoads = {getDriverLibDir() /
                                                   "libopenvino_intel_npu_compiler_loader.so",
                                               "libnpu_driver_compiler.so"};
        std::string errorMsg;
        for (auto name : libToLoads) {
            handle = VclHandle(dlopen(name.c_str(), RTLD_LAZY | RTLD_LOCAL), &closeHandle);
            if (handle)
                break;

            errorMsg += std::string("\ndlopen: ") + name + ", error: " + dlerror();
        }

        if (!handle) {
            LOG_E("Failed to load driver compiler:%s", errorMsg.c_str());
            return;
        }

        compilerCreate = getSymbolAddr<decltype(compilerCreate)>("vclCompilerCreate");
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
        allocatedExecutableCreate2 =
            getSymbolAddr<decltype(allocatedExecutableCreate2)>("vclAllocatedExecutableCreate2");
        getVersion = getSymbolAddr<decltype(getVersion)>("vclGetVersion");
        getCompilerSupportedOptions = getSymbolAddrOrNullptr<decltype(getCompilerSupportedOptions)>(
            "vclGetCompilerSupportedOptions");
        getCompilerIsOptionSupported =
            getSymbolAddrOrNullptr<decltype(getCompilerIsOptionSupported)>(
                "vclGetCompilerIsOptionSupported");
    }

    static void closeHandle(void *handle) noexcept { dlclose(handle); }

  public:
    decltype(vclCompilerCreate) *compilerCreate = &missingSymbol;
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
    decltype(vclAllocatedExecutableCreate2) *allocatedExecutableCreate2 = &missingSymbol;
    decltype(vclGetVersion) *getVersion = &missingSymbol;
    decltype(vclGetCompilerSupportedOptions) *getCompilerSupportedOptions = nullptr;
    decltype(vclGetCompilerIsOptionSupported) *getCompilerIsOptionSupported = nullptr;

  private:
    using VclHandle = std::unique_ptr<void, decltype(&closeHandle)>;
    VclHandle handle = VclHandle(nullptr, nullptr);
};
