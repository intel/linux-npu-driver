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

    template <typename T>
    T getSymbolAddrOrNullptr(const char *name) {
        void *sym = dlsym(handle.get(), name);
        if (!sym) {
            LOG_W("Failed to load %s symbol, error: %s", name, dlerror());
            return nullptr;
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
        allocatedExecutableCreate2 = getSymbolAddrOrNullptr<decltype(allocatedExecutableCreate2)>(
            "vclAllocatedExecutableCreate2");
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
    decltype(vclAllocatedExecutableCreate) *allocatedExecutableCreate = &missingSymbol;
    decltype(vclAllocatedExecutableCreate2) *allocatedExecutableCreate2 = nullptr;
    decltype(vclGetVersion) *getVersion = &missingSymbol;
    decltype(vclGetCompilerSupportedOptions) *getCompilerSupportedOptions = nullptr;
    decltype(vclGetCompilerIsOptionSupported) *getCompilerIsOptionSupported = nullptr;

  private:
    using VclHandle = std::unique_ptr<void, decltype(&closeHandle)>;
    VclHandle handle = VclHandle(nullptr, nullptr);

    std::array<const char *, 1> compilerNames = {"libnpu_driver_compiler.so"};
};
