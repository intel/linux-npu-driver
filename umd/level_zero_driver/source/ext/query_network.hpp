/*
 * Copyright (C) 2023-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>

#include "level_zero/ze_graph_ext.h"
#include "npu_driver_compiler.h"

#include <level_zero/ze_api.h>

struct _ze_graph_query_network_handle_t {};

namespace L0 {

struct QueryNetwork : _ze_graph_query_network_handle_t {
  public:
    static ze_result_t create(ze_context_handle_t hContext,
                              ze_device_handle_t hDevice,
                              const ze_graph_desc_2_t *desc,
                              ze_graph_query_network_handle_t *phGraphQueryNetwork);

    ze_result_t destroy();

    ze_result_t getSupportedLayers(size_t *pSize, char *pSupportedLayers);

    static QueryNetwork *fromHandle(ze_graph_query_network_handle_t handle) {
        return static_cast<QueryNetwork *>(handle);
    }

    inline ze_graph_query_network_handle_t toHandle() { return this; }

  private:
    QueryNetwork(vcl_compiler_handle_t compiler, vcl_query_handle_t query)
        : compiler(compiler)
        , query(query) {}

    vcl_compiler_handle_t compiler;
    vcl_query_handle_t query;
};

} // namespace L0
