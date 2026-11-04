/*
 * Copyright (C) 2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 * @file ze_loader.h
 */

#ifndef _ZE_LOADER_H
#define _ZE_LOADER_H
#if defined(__cplusplus)
#pragma once
#endif

#include "../ze_api.h"
#include "../layers/zel_tracing_register_cb.h"

#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

 typedef struct _zel_version {
    int major;
    int minor;
    int patch; 
  } zel_version_t; 

 //Ex component string "ze_tracing", "ze_validation", etc 
#define ZEL_COMPONENT_STRING_SIZE 64 

 typedef struct zel_component_version {
    char component_name[ZEL_COMPONENT_STRING_SIZE];
    ze_api_version_t spec_version;
    zel_version_t component_lib_version;
} zel_component_version_t; 


/**
 * @brief Retrieves version information for all components of the Level Zero loader.
 *
 * This function returns the versions of all loader components. It requires that
 * the driver initialization routine has been called prior to use.
 *
 * Preconditions:
 * - `zeInitDrivers()` must be successfully invoked before calling this function.
 *
 * Usage:
 * - If `versions` is `NULL`, the function writes the number of available component
 *   versions to `*num_elems` and returns.
 * - If `versions` is non-NULL, provide an array large enough to hold `*num_elems`
 *   entries; the function fills the array with the versions of all loader components.
 *
 * @param[out] num_elems
 *   Pointer to a size_t that receives the number of available component version entries.
 *   When `versions` is non-NULL, on success it may be updated to reflect the number
 *   of entries written.
 *
 * @param[out] versions
 *   Pointer to an array of `zel_component_version_t` to be filled with version data.
 *   If set to `NULL`, no version data is returned and only `*num_elems` is populated.
 *
 * @return ze_result_t
 *   - `ZE_RESULT_SUCCESS` on successful retrieval.
 *   - `ZE_RESULT_ERROR_UNINITIALIZED` if `zeInitDrivers()` was not called.
 *   - `ZE_RESULT_ERROR_INVALID_NULL_POINTER` if required pointers are `NULL`.
 *   - `ZE_RESULT_ERROR_INVALID_SIZE` if the provided array is too small.
 *   - Other `ze_result_t` error codes as appropriate.
 */
ZE_APIEXPORT ze_result_t ZE_APICALL
zelLoaderGetVersions(
   size_t *num_elems,                     //Pointer to num versions to get.  
   zel_component_version_t *versions);    //Pointer to array of versions. If set to NULL, num_elems is returned


/**
 * @brief Retrieves the Level Zero loader's version information.
 *
 * Populates the provided zel_component_version_t structure with the loader's
 * version details (e.g., major, minor, patch), allowing applications to
 * query and validate the loader's compatibility at runtime.
 *
 * Does not require zeInit() or zeInitDrivers() to be called prior to invocation.
 * Works with both static and dynamic loader builds without initialization.
 *
 * Thread-safety: This function is safe to call from multiple threads.
 * The implementation does not modify global state other than filling the
 * supplied version structure.
 *
 * @param[out] version
 *   Pointer to a zel_component_version_t structure that will be filled with
 *   the loader's version information. Must be a valid, non-null pointer.
 *
 * @return
 *   - ZE_RESULT_SUCCESS on successful retrieval of the loader version.
 *   - ZE_RESULT_ERROR_INVALID_NULL_POINTER if version is nullptr.
 *   - ZE_RESULT_ERROR_UNINITIALIZED if the loader library cannot be found or loaded
 *     (only possible in static builds with misconfigured library paths).
 *   - Other ze_result_t error codes on failure conditions as defined by the API.
 *
 * @note The caller owns the memory for the version structure and must ensure
 *       it remains valid for the duration of the call.
 */
ZE_DLLEXPORT ze_result_t ZE_APICALL
zelGetLoaderVersion(zel_component_version_t *version
);

typedef enum _zel_handle_type_t {
   ZEL_HANDLE_DRIVER,
   ZEL_HANDLE_DEVICE,
   ZEL_HANDLE_CONTEXT,
   ZEL_HANDLE_COMMAND_QUEUE,
   ZEL_HANDLE_COMMAND_LIST,
   ZEL_HANDLE_FENCE,
   ZEL_HANDLE_EVENT_POOL,
   ZEL_HANDLE_EVENT,
   ZEL_HANDLE_IMAGE,
   ZEL_HANDLE_MODULE,
   ZEL_HANDLE_MODULE_BUILD_LOG,
   ZEL_HANDLE_KERNEL,
   ZEL_HANDLE_SAMPLER,
   ZEL_HANDLE_PHYSICAL_MEM
} zel_handle_type_t;

/**
 * @brief Translates loader handles to driver handles when handle interception is enabled.
 *
 * This function provides handle translation for scenarios where the loader intercepts
 * and wraps driver handles. When handle interception is enabled, the loader maintains
 * a mapping between loader-visible handles and underlying driver handles. This function
 * performs the translation from loader handles to their corresponding driver handles.
 *
 * Handle interception is typically used by validation layers, tracing layers, or other
 * middleware that needs to track handle lifecycles and API usage patterns.
 *
 * Behavior:
 * - If handle interception is enabled: Translates handleIn to the corresponding driver handle
 *   and stores the result in *handleOut.
 * - If handle interception is disabled: Sets *handleOut equal to handleIn (pass-through).
 *
 * Thread-safety: This function is thread-safe and can be called concurrently from multiple
 * threads with different handle values.
 *
 * @param[in] handleType
 *   The type of handle being translated, specified using zel_handle_type_t enumeration.
 *   This indicates whether the handle is a driver, device, context, command queue, etc.
 *
 * @param[in] handleIn
 *   The loader handle to translate. This is the handle visible to the application or layer.
 *   Must be a valid handle of the type specified by handleType, or NULL.
 *
 * @param[out] handleOut
 *   Pointer to a void* that will receive the driver handle. If handle interception is enabled,
 *   this will be set to the underlying driver handle. If disabled, it will be set to handleIn.
 *   Must be a valid, non-null pointer.
 *
 * @return
 *   - ZE_RESULT_SUCCESS if the translation was successful.
 *   - ZE_RESULT_ERROR_INVALID_NULL_POINTER if handleOut is null.
 *   - ZE_RESULT_ERROR_INVALID_ARGUMENT if handleType is invalid or handleIn is invalid.
 *   - Other ze_result_t error codes as appropriate.
 *
 * @note If handleIn is NULL, *handleOut will be set to NULL regardless of interception state.
 */
ZE_APIEXPORT ze_result_t ZE_APICALL
zelLoaderTranslateHandle(
   zel_handle_type_t handleType,
   void *handleIn,
   void **handleOut);

/**
 * @brief Notifies the loader that a driver has been removed and forces prevention of subsequent API calls.
 *
 * This function is intended to be called ONLY by Level Zero drivers, not by applications.
 * It signals to the loader that at least one driver has been removed from the application
 * environment, forcing the loader into a protective state that will prevent subsequent calls
 * to Level Zero APIs to avoid crashes or undefined behavior when accessing freed driver resources.
 *
 * When a driver calls this function, the loader will:
 * - Enter a special teardown state that blocks new API calls
 * - Track that at least one driver has been removed from the environment
 * - Return appropriate error codes (typically ZE_RESULT_ERROR_UNINITIALIZED) for subsequent API calls
 * - Prevent crashes from accessing freed driver resources during teardown
 *
 * Use Case:
 * This function exists to handle scenarios where drivers are dynamically unloaded or released
 * (e.g., during process teardown, driver updates, or hot-plugging) while the application may
 * still attempt to make Level Zero API calls. By forcing the loader into this protective state,
 * drivers can ensure safe shutdown even when application threads are not fully synchronized.
 *
 * This function should be called by the driver after zeInit() or zeInitDrivers() has been
 * successfully invoked, typically during the driver's own teardown or cleanup sequence.
 *
 * Typical usage (by driver implementation):
 *   // In driver teardown code:
 *   zelSetDriverTeardown();  // Signal loader that driver is being removed
 *   // Continue with driver cleanup...
 *
 * Thread-safety: This function is thread-safe and can be called from any thread. Once called,
 * the loader state change is global and affects all subsequent API calls across all threads.
 *
 * @return
 *   - ZE_RESULT_SUCCESS if the loader was successfully placed into teardown prevention mode.
 *   - ZE_RESULT_ERROR_UNINITIALIZED if zeInit() or zeInitDrivers() was not called.
 *   - ZE_RESULT_ERROR_UNSUPPORTED if the current loader configuration does not support this feature.
 *   - Other ze_result_t error codes as appropriate.
 *
 * @warning This function is for DRIVER USE ONLY. Applications should NOT call this function.
 *          Calling this from application code will cause all subsequent Level Zero API calls
 *          to fail, effectively disabling Level Zero functionality for the remainder of the
 *          process lifetime.
 *
 * @note Once this function is called, the loader enters an irreversible state where API calls
 *       are prevented. The loader cannot be re-initialized after this function has been invoked.
 *
 * @note This function only affects the loader's handling of API calls. It does not directly
 *       manage driver resources or perform driver cleanup.
 */
ZE_DLLEXPORT ze_result_t ZE_APICALL
zelSetDriverTeardown(void);

/**
 * @brief Delays automatic loader context teardown until explicitly requested.
 *
 * This function instructs the loader to postpone its automatic context teardown sequence,
 * allowing the application to continue using Level Zero APIs during the application's
 * own teardown process. The loader context will remain active until explicitly torn down
 * via a call to zelLoaderContextTeardown().
 *
 * Applicability:
 * - This function ONLY applies to static loader builds.
 * - In dynamic loader builds, this function has no effect.
 *
 * Use Cases:
 * This is a workaround for legacy software stacks that:
 * - Use Level Zero APIs during application teardown (e.g., in global destructors)
 * - Have complex teardown sequences where L0 resources need to outlive other components
 * - Cannot easily be refactored to complete all L0 API calls before teardown
 *
 * Without this function, the static loader's context may be automatically destroyed during
 * process teardown (e.g., via atexit handlers or destructor ordering), potentially before
 * the application has finished using Level Zero APIs. This can lead to crashes or undefined
 * behavior if L0 APIs are called after the context is destroyed.
 *
 * Call Sequence:
 *   zelSetDelayLoaderContextTeardown();  // Delay automatic teardown
 *   // Application uses L0 APIs during teardown...
 *   zelLoaderContextTeardown();          // Explicitly tear down when done
 *
 * Thread-safety: This function should be called from the main thread before any teardown
 * begins. It is not thread-safe to call this concurrently with other loader operations.
 *
 * @warning This function is a workaround for legacy code and should NOT be used in new
 *          applications. The recommended practice is to ensure all Level Zero API calls
 *          complete before application teardown begins. Delaying loader teardown can
 *          complicate resource cleanup and may mask underlying architectural issues.
 *
 * @note This function does not return a result. Once called, the loader context will
 *       remain active until zelLoaderContextTeardown() is explicitly invoked.
 */
ZE_DLLEXPORT void ZE_APICALL
zelSetDelayLoaderContextTeardown(void);

/**
 * @brief Explicitly tears down the loader's context and releases all associated resources.
 *
 * This function performs a complete teardown of the loader's internal context, including:
 * - Releasing all driver handles and associated resources
 * - Unloading driver libraries
 * - Freeing internal data structures and caches
 * - Invoking registered teardown callbacks
 * - Cleaning up inter-process communication resources (if applicable)
 *
 * Applicability:
 * - This function ONLY applies to static loader builds.
 * - In dynamic loader builds, teardown occurs automatically when the loader library is unloaded.
 *
 * Preconditions:
 * - This function should only be called after zelSetDelayLoaderContextTeardown() has been invoked.
 * - All Level Zero API calls must be completed before calling this function.
 * - All application threads using Level Zero APIs must have finished execution.
 *
 * Post-conditions:
 * - After this function returns, no Level Zero APIs can be called.
 * - Attempting to use L0 APIs after teardown results in undefined behavior (typically crashes).
 * - The loader context cannot be re-initialized after teardown.
 *
 * Typical Usage Pattern:
 *   // During application initialization:
 *   zelSetDelayLoaderContextTeardown();
 *   
 *   // Use Level Zero APIs throughout application lifetime...
 *   
 *   // During application teardown:
 *   // Ensure all L0 operations complete
 *   // Wait for all threads using L0 to finish
 *   zelLoaderContextTeardown();  // Explicit teardown
 *
 * Thread-safety: This function is NOT thread-safe and must be called from a single thread
 * when no other threads are executing Level Zero APIs. Calling this function concurrently
 * with L0 API usage will result in undefined behavior.
 *
 * @warning This function is a workaround for legacy code patterns and should NOT be used
 *          in new applications. Modern applications should rely on automatic teardown by
 *          ensuring all L0 API usage completes before process termination. Using explicit
 *          teardown requires careful coordination of application shutdown sequences.
 *
 * @note This function does not return a result. Errors during teardown are logged but
 *       do not prevent the teardown from completing. Once called, the loader context
 *       is considered invalid regardless of any internal errors encountered.
 */
ZE_DLLEXPORT void ZE_APICALL
zelLoaderContextTeardown(void);

/**
 * @brief Enables the Level Zero tracing layer at runtime.
 *
 * This function activates the tracing layer, which intercepts and logs Level Zero API calls.
 * The tracing layer is useful for debugging, performance analysis, and understanding the
 * sequence and parameters of API calls made by an application.
 *
 * When enabled, the tracing layer:
 * - Intercepts all Level Zero API calls (ze*, zes*, zet*, zer* functions)
 * - Logs function entry and exit points
 * - Records function parameters and return values
 * - Measures API call timing and performance metrics
 * - Provides hooks for custom tracing callbacks (if registered)
 *
 * The tracing layer can be enabled at any point during application execution, but is most
 * commonly enabled during initialization or before a specific code region of interest.
 *
 * Runtime vs Build-time Configuration:
 * - This function enables tracing at runtime, overriding build-time or environment settings.
 * - Tracing can also be enabled via environment variables (e.g., ZE_ENABLE_TRACING_LAYER).
 * - Runtime enabling via this function takes precedence over environment configuration.
 *
 * Preconditions:
 * - The tracing layer library must be available and loadable by the loader.
 * - For best results, call this function after zeInit() or zeInitDrivers() and around the specific code region of interest.
 * - Can be called after initialization, but will only affect subsequent API calls.
 *
 * Performance Considerations:
 * - Enabling tracing adds overhead to every API call (typically 1-10 microseconds per call).
 * - Applications should disable tracing in performance-critical production code.
 * - Tracing overhead varies based on logging verbosity and callback complexity.
 *
 * Thread-safety: This function is thread-safe and can be called from any thread. However,
 * enabling tracing while other threads are actively making API calls may result in some
 * calls not being traced if they were already in progress.
 *
 * @return
 *   - ZE_RESULT_SUCCESS if the tracing layer was successfully enabled.
 *   - ZE_RESULT_ERROR_UNINITIALIZED if the loader has not been properly initialized.
 *   - ZE_RESULT_ERROR_UNSUPPORTED if the tracing layer is not available (missing library).
 *   - ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE if tracing dependencies cannot be loaded.
 *   - ZE_RESULT_SUCCESS if tracing is already enabled (idempotent operation).
 *   - Other ze_result_t error codes as appropriate.
 *
 * @see zelDisableTracingLayer() to disable tracing
 * @see zelGetTracingLayerState() to query current tracing state
 */
ZE_DLLEXPORT ze_result_t ZE_APICALL
zelEnableTracingLayer(void);

/**
 * @brief Checks whether the loader is currently in teardown state.
 *
 * This function provides a way for applications, layers, and drivers to query whether
 * the loader is currently executing its teardown sequence. This information is useful
 * for making decisions about resource cleanup, avoiding operations that may fail during
 * teardown, and implementing safe shutdown logic in complex multi-threaded applications.
 *
 * Teardown State:
 * The loader enters teardown state when:
 * - The process is terminating (exit() called or main() returns)
 * - zelLoaderContextTeardown() has been explicitly called
 * - atexit handlers or global destructors are executing
 * - Dynamic loader library is being unloaded (dlclose() or FreeLibrary())
 *
 * Use Cases:
 * - Preventing new API calls during shutdown to avoid crashes
 * - Implementing conditional cleanup logic in global destructors
 * - Deciding whether to log errors or silently fail during teardown
 * - Coordinating shutdown sequences across multiple components
 * - Avoiding deadlocks by skipping synchronization during teardown
 *
 * Typical Usage:
 *   if (zelCheckIsLoaderInTearDown()) {
 *       // Loader is shutting down - skip operation or use simplified cleanup
 *       return;
 *   }
 *   // Normal operation - proceed with API calls
 *   zeDeviceGet(...);
 *
 * Thread-safety: This function is thread-safe and can be called from any thread at any
 * time. It provides a snapshot of the loader's teardown state at the moment of the call.
 * The state may change immediately after the function returns if teardown begins on
 * another thread.
 *
 * Performance: This is a lightweight query operation with minimal overhead, suitable for
 * frequent checking in performance-sensitive code paths.
 *
 * @return
 *   - true if the loader is currently in teardown state or has completed teardown.
 *   - false if the loader is in normal operational state.
 *
 * @note Race conditions: In multi-threaded scenarios, teardown may begin immediately
 *       after this function returns false. Callers should be prepared to handle errors
 *       from subsequent API calls even if this function returns false.
 *
 * @note During teardown, most Level Zero API calls will return error codes or exhibit
 *       undefined behavior. Applications should avoid making API calls when this function
 *       returns true.
 */
ZE_DLLEXPORT bool ZE_APICALL
zelCheckIsLoaderInTearDown(void);

/**
 * @brief Function pointer type for application-provided teardown callbacks.
 *
 * This typedef defines the signature for callback functions that applications can register
 * to be notified when the loader begins its teardown sequence. The loader invokes these
 * callbacks to give applications an opportunity to perform cleanup, save state, or prepare
 * for shutdown before the loader's internal resources are released.
 *
 * Callback Requirements:
 * - Must not take any parameters
 * - Must not return any value (void return type)
 * - Must be thread-safe (may be called from any thread)
 * - Must not block or take locks that could deadlock the teardown sequence
 * - Should complete quickly (ideally < 100 microseconds)
 * - Must not call Level Zero APIs (may result in undefined behavior or deadlock)
 *
 * Implementation Guidelines:
 * The callback should perform minimal work, typically limited to:
 * - Setting flags or updating state variables
 * - Signaling condition variables or event objects
 * - Initiating asynchronous cleanup on other threads
 * - Logging or diagnostic output
 *
 * What NOT to do in callbacks:
 * - DO NOT call Level Zero APIs (ze*, zes*, zet*, zer* functions)
 * - DO NOT allocate or free memory (may deadlock during process teardown)
 * - DO NOT acquire locks (risk of deadlock with loader's internal locks)
 * - DO NOT perform I/O operations (may block or fail during shutdown)
 * - DO NOT start new threads or wait for thread completion
 *
 * Example Implementation:
 *   static volatile bool loader_teardown_started = false;
 *   
 *   void myTeardownCallback() {
 *       loader_teardown_started = true;  // Simple flag update
 *   }
 *
 * @see zelRegisterTeardownCallback() for registering callbacks
 */
typedef void (*zel_loader_teardown_callback_t)(void);

/**
 * @brief Function pointer type for loader-provided callbacks to notify application of teardown.
 *
 * This typedef defines the signature for callbacks that the loader provides to applications
 * during teardown callback registration. The loader uses this callback mechanism to notify
 * the application when teardown is complete or to coordinate multi-stage shutdown sequences.
 *
 * Unlike zel_loader_teardown_callback_t (which applications provide to the loader), this
 * callback type is implemented by the loader and given to the application. The application
 * stores this callback and may invoke it at appropriate points during its own shutdown.
 *
 * Parameter:
 * @param index
 *   The index value assigned to the registered callback during zelRegisterTeardownCallback().
 *   This allows the loader to identify which callback invocation is being acknowledged or
 *   to coordinate multiple registered callbacks in a specific order.
 *
 * Usage Pattern:
 *   zel_application_teardown_callback_t loader_callback;
 *   uint32_t callback_index;
 *   
 *   zelRegisterTeardownCallback(my_callback, &loader_callback, &callback_index);
 *   
 *   // Later, during application teardown:
 *   if (loader_callback != nullptr) {
 *       loader_callback(callback_index);  // Notify loader of application teardown progress
 *   }
 *
 * Thread-safety: The implementation of this callback (provided by the loader) is thread-safe
 * and can be called from any thread. However, applications should typically call it from
 * the same thread that is coordinating shutdown.
 *
 * Callback Requirements:
 * - Must be safe to call during application teardown
 * - Must not block indefinitely
 * - May be called multiple times with the same index (idempotent)
 * - May be nullptr if the loader does not require application notification
 *
 * @note This callback is part of the bidirectional communication mechanism between the
 *       application and loader during teardown. The loader calls the application's callback
 *       (zel_loader_teardown_callback_t) to signal loader teardown, and the application
 *       calls the loader's callback (this type) to acknowledge or coordinate its own teardown.
 *
 * @see zelRegisterTeardownCallback() for the complete callback registration mechanism
 */
typedef void (*zel_application_teardown_callback_t)(uint32_t index);

/**
 * @brief Registers a teardown callback to be invoked during loader teardown.
 *
 * This function allows the application to register a callback function that will be called
 * when the loader is being torn down. The loader will also provide its own callback function
 * and assign an index to the registered callback.
 *
 * The application_callback is required to be a function that takes no arguments and returns void.
 * In addition, the application_callback should be thread-safe and not block to prevent deadlocking the
 * loader teardown process.
 *
 * For example, the application_callback used by the static loader is:
 *  void staticLoaderTeardownCallback() {
 *    loaderTeardownCallbackReceived = true;
 *  }
 * The application_callback should provide a simple notification to the application that the loader is being torn down.
 *
 * @param[in] application_callback  Application's callback function to be called during loader teardown.
 * @param[out] loader_callback      Pointer to the loader's callback function.
 * @param[out] index                Index assigned to the registered callback.
 *
 * @return
 *     - ZE_RESULT_SUCCESS if the callback was successfully registered.
 *     - Appropriate error code otherwise.
 */
ZE_DLLEXPORT ze_result_t ZE_APICALL
zelRegisterTeardownCallback(
   zel_loader_teardown_callback_t application_callback, // [in] Application's callback function to be called during loader teardown
   zel_application_teardown_callback_t *loader_callback, // [out] Pointer to the loader's callback function
   uint32_t *index // [out] Index assigned to the registered callback
);

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for Disabling the Tracing Layer During Runtime.
///
ZE_DLLEXPORT ze_result_t ZE_APICALL
zelDisableTracingLayer(void);

/**
 * @brief Retrieves the current enabled state of the Level Zero tracing layer.
 *
 * This function queries whether the tracing layer is active and writes the result
 * to the provided boolean pointer.
 *
 * @param enabled
 *        Pointer to a boolean that will be set to true if the tracing layer is
 *        currently enabled, or false if it is disabled. Must be a valid, non-null
 *        pointer.
 *
 * @return
 *        ZE_RESULT_SUCCESS on success.
 *        ZE_RESULT_ERROR_INVALID_NULL_POINTER if `enabled` is null.
 *        Other ze_result_t error codes may be returned for implementation-specific failures.
 *
 * @note The tracing layer state is global to the process. The function is read-only
 *       and thread-safe; multiple callers can query the state concurrently.
 */
ZE_DLLEXPORT ze_result_t ZE_APICALL
zelGetTracingLayerState(bool* enabled); // Pointer to bool to receive tracing layer state

///////////////////////////////////////////////////////////////////////////////
/// @brief Callback signature for extension-function prologue/epilogue handlers.
///
/// This intentionally mirrors the established per-API tracing callback shape
/// (see the ze_pfnXCb_t typedefs in ze_api.h) so tools can reuse their existing
/// callback infrastructure. Because an arbitrary extension function has no
/// generated params struct, @p pParams is passed as an opaque void* whose layout
/// is defined by the driver for the named function (may be null for pure-vendor
/// functions). The identity of the fired function is carried via
/// @p pTracerUserData (set at registration time).
///
/// @param[in] pParams                    driver-populated parameter block (opaque)
/// @param[in] result                     epilogue only: the function's return value
/// @param[in] pTracerUserData            per-registration user data
/// @param[in,out] ppTracerInstanceUserData  per-call scratch for prologue->epilogue handoff
typedef void (ZE_APICALL *zel_pfnDriverExtensionFunctionCb_t)(
    void* pParams,
    ze_result_t result,
    void* pTracerUserData,
    void** ppTracerInstanceUserData
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Signature of the per-driver hook that enables or disables the driver's
///        extension-function callbacks.
///
/// A driver that supports extension-function tracing exposes this by name
/// ("zelDriverEnableTracing") via zeDriverGetExtensionFunctionAddress. The loader
/// calls it on each active driver when the tracing layer is enabled/disabled
/// (including static ZE_ENABLE_TRACING_LAYER enablement and late-loaded drivers).
/// When disabled, the driver must not invoke any registered prologue/epilogue.
typedef ze_result_t (ZE_APICALL *zel_pfnDriverEnableTracing_t)(
    ze_driver_handle_t hDriver,
    ze_bool_t enable
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Signature of the per-driver hook the loader/tracing-layer uses to
///        install its extension-function interception wrappers on a driver.
///
/// A driver that supports extension-function tracing exposes this by name
/// ("zelDriverSetLoaderCallbackForExtension") via
/// zeDriverGetExtensionFunctionAddress. The tracing layer calls it to register a
/// single loader-owned prologue/epilogue wrapper (plus an opaque loader context)
/// for the named extension function. The driver invokes @p loaderPrologue before,
/// and @p loaderEpilogue after, the body of the extension function named
/// @p functionName, forwarding @p pLoaderContext back unchanged. Passing null for
/// both wrappers unregisters. The loader owns the fan-out to any number of
/// registered tracers, so the driver stores at most one wrapper per function.
typedef ze_result_t (ZE_APICALL *zel_pfnDriverSetLoaderCallbackForExtension_t)(
    ze_driver_handle_t hDriver,                      // [in] handle of the driver instance
    const char* functionName,                        // [in] extension function name to intercept
    zel_pfnDriverExtensionFunctionCb_t loaderPrologue, // [in][optional] loader prologue wrapper
    zel_pfnDriverExtensionFunctionCb_t loaderEpilogue, // [in][optional] loader epilogue wrapper
    void* pLoaderContext                             // [in][optional] loader context echoed to wrappers
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Registers a prologue or epilogue callback on a tracer for a named
///        extension function of a specific driver.
///
/// Extension functions obtained by string name via
/// zeDriverGetExtensionFunctionAddress() return a raw driver pointer that the
/// application calls directly, bypassing the loader and therefore the per-API
/// tracing interceptors. This API routes such functions through the same tracer
/// (::zel_tracer_handle_t) infrastructure used for core APIs: the tracing layer
/// installs a loader-owned wrapper on @p hDriver (via the driver's
/// zelDriverSetLoaderCallbackForExtension hook) and fans out to every enabled
/// tracer that registered @p functionName for @p hDriver.
///
/// Registration is keyed by (@p hDriver, @p functionName) and is order-independent
/// relative to zeDriverGetExtensionFunctionAddress() — it takes effect on the next
/// invocation even if the application already cached the function pointer. The
/// callback receives the tracer's pUserData (from ::zelTracerCreate) as
/// pTracerUserData. Multiple tracers may register the same function to stack
/// callbacks. The callbacks fire only when the tracing layer is enabled for the
/// driver and the tracer is enabled.
///
/// @param[in] hTracer        handle of the tracer to register the callback on
/// @param[in] hDriver        handle of the driver whose extension function to trace
/// @param[in] functionName   name of the extension function to intercept
/// @param[in] callback_type  ::ZEL_REGISTER_PROLOGUE or ::ZEL_REGISTER_EPILOGUE
/// @param[in] pCallback      handler to register (null clears that slot)
///
/// @return
///   - ZE_RESULT_SUCCESS on success (including clearing a slot).
///   - ZE_RESULT_ERROR_UNINITIALIZED if the loader/tracing layer is not initialized.
///   - ZE_RESULT_ERROR_UNSUPPORTED_FEATURE if the driver does not implement the hook.
///   - ZE_RESULT_ERROR_INVALID_NULL_HANDLE if @p hTracer or @p hDriver is null.
///   - ZE_RESULT_ERROR_INVALID_NULL_POINTER if @p functionName is null.
///   - ZE_RESULT_ERROR_INVALID_ARGUMENT if the tracer is not in the disabled state.
ZE_DLLEXPORT ze_result_t ZE_APICALL
zelTracerDriverExtensionRegisterCallback(
    zel_tracer_handle_t hTracer,                     // [in] handle of the tracer
    ze_driver_handle_t hDriver,                      // [in] handle of the driver instance
    const char* functionName,                        // [in] extension function name to intercept
    zel_tracer_reg_t callback_type,                  // [in] prologue or epilogue
    zel_pfnDriverExtensionFunctionCb_t pCallback     // [in][optional] handler (null clears slot)
    );

#if defined(__cplusplus)
} // extern "C"
#endif
#endif //_ZE_LOADER_H