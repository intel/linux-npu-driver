/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"

#include <sstream>
#include <string>
#include <uapi/drm/drm.h>
#include <uapi/drm/ivpu_accel.h>

static std::string struct_drm_version_to_str(const drm_version *version) {
    std::ostringstream oss;
    oss << "(version_major: " << version->version_major << ", "
        << "version_minor: " << version->version_minor << ", "
        << "version_patchlevel: " << version->version_patchlevel << ", "
        << "name_len: " << version->name_len << ", "
        << "name: " << (version->name ? version->name : "null") << ", "
        << "date_len: " << version->date_len << ", "
        << "date: " << (version->date ? version->date : "null") << ", "
        << "desc_len: " << version->desc_len << ", "
        << "desc: " << (version->desc ? version->desc : "null") << ")";
    return oss.str();
}

static std::string struct_drm_get_cap_to_str(const drm_get_cap *get_cap) {
    std::ostringstream oss;
    oss << std::hex << std::showbase << "(capability: " << get_cap->capability
        << ", value: " << get_cap->value << ")";
    return oss.str();
}

static std::string struct_drm_gem_close_to_str(const drm_gem_close *gem_close) {
    std::ostringstream oss;
    oss << "(handle: " << gem_close->handle << ")";
    return oss.str();
}

static std::string struct_drm_prime_handle_to_str(const drm_prime_handle *prime_handle) {
    std::ostringstream oss;
    oss << "(handle: " << prime_handle->handle << ", "
        << "flags: " << prime_handle->flags << ", "
        << "fd: " << prime_handle->fd << ")";
    return oss.str();
}

static const char *struct_param_name(unsigned int param) {
    switch (param) {
        CASE_RETURN_STR(DRM_IVPU_PARAM_DEVICE_ID);
        CASE_RETURN_STR(DRM_IVPU_PARAM_DEVICE_REVISION);
        CASE_RETURN_STR(DRM_IVPU_PARAM_PLATFORM_TYPE);
        CASE_RETURN_STR(DRM_IVPU_PARAM_CORE_CLOCK_RATE);
        CASE_RETURN_STR(DRM_IVPU_PARAM_NUM_CONTEXTS);
        CASE_RETURN_STR(DRM_IVPU_PARAM_CONTEXT_PRIORITY);
        CASE_RETURN_STR(DRM_IVPU_PARAM_CONTEXT_ID);
        CASE_RETURN_STR(DRM_IVPU_PARAM_FW_API_VERSION);
        CASE_RETURN_STR(DRM_IVPU_PARAM_ENGINE_HEARTBEAT);
        CASE_RETURN_STR(DRM_IVPU_PARAM_UNIQUE_INFERENCE_ID);
        CASE_RETURN_STR(DRM_IVPU_PARAM_TILE_CONFIG);
        CASE_RETURN_STR(DRM_IVPU_PARAM_SKU);
        CASE_RETURN_STR(DRM_IVPU_PARAM_CAPABILITIES);
        CASE_RETURN_STR(DRM_IVPU_PARAM_PREEMPT_BUFFER_SIZE);
    default:
        return "Unknown";
    }
}

const char *driver_struct_param_cap_index_str(unsigned index) {
    switch (index) {
        CASE_RETURN_STR(DRM_IVPU_CAP_METRIC_STREAMER);
        CASE_RETURN_STR(DRM_IVPU_CAP_DMA_MEMORY_RANGE);
        CASE_RETURN_STR(DRM_IVPU_CAP_MANAGE_CMDQ);
        CASE_RETURN_STR(DRM_IVPU_CAP_BO_CREATE_FROM_USERPTR);
    default:
        return "Unknown";
    }
}

static std::string struct_drm_ivpu_param_to_str(const drm_ivpu_param *param) {
    std::ostringstream oss;
    oss << "(param: " << struct_param_name(param->param) << " (" << param->param << "), ";
    if (param->param == DRM_IVPU_PARAM_CAPABILITIES) {
        oss << "index: " << driver_struct_param_cap_index_str(param->index) << " (" << param->index
            << "), ";
    } else {
        oss << "index: " << param->index << ", ";
    }
    oss << "value: " << param->value << ")";
    return oss.str();
}

static std::string struct_drm_ivpu_bo_create_to_str(const drm_ivpu_bo_create *bo_create) {
    std::ostringstream oss;
    oss << std::hex << std::showbase << "(size: " << bo_create->size << ", "
        << "flags: " << bo_create->flags << ", "
        << "handle: " << bo_create->handle << ", "
        << "vpu_addr: " << bo_create->vpu_addr << ")";
    return oss.str();
}

static std::string struct_drm_ivpu_bo_create_from_userptr_to_str(
    const drm_ivpu_bo_create_from_userptr *bo_create_from_userptr) {
    std::ostringstream oss;
    oss << std::hex << std::showbase << "(userptr: " << bo_create_from_userptr->user_ptr << ", "
        << "size: " << bo_create_from_userptr->size << ", "
        << "flags: " << bo_create_from_userptr->flags << ", "
        << "handle: " << bo_create_from_userptr->handle << ", "
        << "vpu_addr: " << bo_create_from_userptr->vpu_addr << ")";
    return oss.str();
}

static std::string struct_drm_ivpu_bo_info_to_str(const drm_ivpu_bo_info *bo_info) {
    std::ostringstream oss;
    oss << "(handle: " << bo_info->handle << ", " << std::hex << std::showbase
        << "flags: " << bo_info->flags << ", "
        << "vpu_addr: " << bo_info->vpu_addr << ", "
        << "mmap_offset: " << bo_info->mmap_offset << ", "
        << "size: " << bo_info->size << ")";
    return oss.str();
}

static const char *struct_drm_ivpu_job_priority_to_str(unsigned priority) {
    switch (priority) {
        CASE_RETURN_STR(DRM_IVPU_JOB_PRIORITY_DEFAULT);
        CASE_RETURN_STR(DRM_IVPU_JOB_PRIORITY_IDLE);
        CASE_RETURN_STR(DRM_IVPU_JOB_PRIORITY_NORMAL);
        CASE_RETURN_STR(DRM_IVPU_JOB_PRIORITY_FOCUS);
        CASE_RETURN_STR(DRM_IVPU_JOB_PRIORITY_REALTIME);
    default:
        return "Unknown";
    }
}

static std::string struct_drm_ivpu_submit_to_str(const drm_ivpu_submit *submit) {
    std::ostringstream oss;
    oss << std::hex << std::showbase << "(buffers_ptr: " << submit->buffers_ptr << ", " << std::dec
        << "buffer_count: " << submit->buffer_count << ", "
        << "engine: " << submit->engine << ", "
        << "flags: " << submit->flags << ", "
        << "commands_offset: " << submit->commands_offset << ", "
        << "priority: " << struct_drm_ivpu_job_priority_to_str(submit->priority) << " ("
        << submit->priority << ")";
    return oss.str();
}

static std::string struct_drm_ivpu_cmdq_submit_to_str(const drm_ivpu_cmdq_submit *cmdq_submit) {
    std::ostringstream oss;
    oss << std::hex << std::showbase << "(buffers_ptr: " << cmdq_submit->buffers_ptr << ", "
        << std::dec << "buffer_count: " << cmdq_submit->buffer_count << ", "
        << "cmdq_id: " << cmdq_submit->cmdq_id << ", " << "flags: " << cmdq_submit->flags << ", "
        << "commands_offset: " << cmdq_submit->commands_offset << ", "
        << "preempt_buffer_index: " << cmdq_submit->preempt_buffer_index << ")";
    return oss.str();
}

static std::string struct_drm_ivpu_bo_wait_to_str(const drm_ivpu_bo_wait *bo_wait) {
    std::ostringstream oss;
    oss << "(handle: " << bo_wait->handle << ", "
        << "flags: " << bo_wait->flags << ", "
        << "timeout_ns: " << bo_wait->timeout_ns << ", "
        << "job_status: " << bo_wait->job_status << ")";
    return oss.str();
}

static std::string
struct_drm_ivpu_metric_streamer_start_to_str(const drm_ivpu_metric_streamer_start *metric_start) {
    std::ostringstream oss;
    oss << std::hex << std::showbase << "(metric_group_mask: " << metric_start->metric_group_mask
        << ", " << std::dec << "sampling_period_ns: " << metric_start->sampling_period_ns << ", "
        << "read_period_samples: " << metric_start->read_period_samples << ", " << std::hex
        << std::showbase << "sample_size: " << metric_start->sample_size << ", "
        << "max_data_size: " << metric_start->max_data_size << ")";
    return oss.str();
}

static std::string struct_drm_ivpu_metric_streamer_get_data_to_str(
    const drm_ivpu_metric_streamer_get_data *metric_data) {
    std::ostringstream oss;
    oss << std::hex << std::showbase << "(metric_group_mask: " << metric_data->metric_group_mask
        << ", "
        << "buffer_ptr: " << metric_data->buffer_ptr << ", "
        << "buffer_size: " << metric_data->buffer_size << ", "
        << "data_size: " << metric_data->data_size << ")";
    return oss.str();
}

static std::string struct_drm_ivpu_cmdq_create_to_str(const drm_ivpu_cmdq_create *cmdq_create) {
    std::ostringstream oss;
    oss << "(cmdq_id: " << cmdq_create->cmdq_id << ", "
        << "priority: " << struct_drm_ivpu_job_priority_to_str(cmdq_create->priority) << " ("
        << cmdq_create->priority << "), " << "flags: " << cmdq_create->flags << ")";
    return oss.str();
}

static std::string struct_drm_ivpu_cmdq_destroy_to_str(const drm_ivpu_cmdq_destroy *cmdq_destroy) {
    std::ostringstream oss;
    oss << "(cmdq_id: " << cmdq_destroy->cmdq_id << ")";
    return oss.str();
}

static std::string
struct_drm_ivpu_metric_streamer_stop_to_str(const drm_ivpu_metric_streamer_stop *metric_stop) {
    std::ostringstream oss;
    oss << "(metric_group_mask: " << metric_stop->metric_group_mask << ")";
    return oss.str();
}

const char *driver_ioctl_request_str(unsigned int request) {
    switch (request) {
        CASE_RETURN_STR(DRM_IOCTL_VERSION);
        CASE_RETURN_STR(DRM_IOCTL_GEM_CLOSE);
        CASE_RETURN_STR(DRM_IOCTL_GET_CAP);
        CASE_RETURN_STR(DRM_IOCTL_PRIME_HANDLE_TO_FD);
        CASE_RETURN_STR(DRM_IOCTL_PRIME_FD_TO_HANDLE);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_GET_PARAM);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_SET_PARAM);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_BO_CREATE);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_BO_INFO);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_SUBMIT);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_BO_WAIT);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_METRIC_STREAMER_START);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_METRIC_STREAMER_STOP);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_METRIC_STREAMER_GET_DATA);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_METRIC_STREAMER_GET_INFO);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_CMDQ_CREATE);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_CMDQ_DESTROY);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_CMDQ_SUBMIT);
        CASE_RETURN_STR(DRM_IOCTL_IVPU_BO_CREATE_FROM_USERPTR);
    default:
        return "IOCTL_UNKNOWN";
    }
}

std::string driver_ioctl_trace(int fd, unsigned int request, void *arg) {
    std::ostringstream oss;
    oss << "fd: " << fd << ", request: ";
    switch (request) {
    case DRM_IOCTL_VERSION:
        oss << "DRM_IOCTL_VERSION, arg:"
            << struct_drm_version_to_str(static_cast<drm_version *>(arg));
        break;
    case DRM_IOCTL_GEM_CLOSE:
        oss << "DRM_IOCTL_GEM_CLOSE, arg:"
            << struct_drm_gem_close_to_str(static_cast<drm_gem_close *>(arg));
        break;
    case DRM_IOCTL_GET_CAP:
        oss << "DRM_IOCTL_GET_CAP, arg:"
            << struct_drm_get_cap_to_str(static_cast<drm_get_cap *>(arg));
        break;
    case DRM_IOCTL_PRIME_HANDLE_TO_FD:
        oss << "DRM_IOCTL_PRIME_HANDLE_TO_FD, arg:"
            << struct_drm_prime_handle_to_str(static_cast<drm_prime_handle *>(arg));
        break;
    case DRM_IOCTL_PRIME_FD_TO_HANDLE:
        oss << "DRM_IOCTL_PRIME_FD_TO_HANDLE, arg:"
            << struct_drm_prime_handle_to_str(static_cast<drm_prime_handle *>(arg));
        break;
    case DRM_IOCTL_IVPU_GET_PARAM:
        oss << "DRM_IOCTL_IVPU_GET_PARAM, arg: "
            << struct_drm_ivpu_param_to_str(static_cast<drm_ivpu_param *>(arg));
        break;
    case DRM_IOCTL_IVPU_SET_PARAM:
        oss << "DRM_IOCTL_IVPU_SET_PARAM, arg: "
            << struct_drm_ivpu_param_to_str(static_cast<drm_ivpu_param *>(arg));
        break;
    case DRM_IOCTL_IVPU_BO_CREATE:
        oss << "DRM_IOCTL_IVPU_BO_CREATE, arg: "
            << struct_drm_ivpu_bo_create_to_str(static_cast<drm_ivpu_bo_create *>(arg));
        break;
    case DRM_IOCTL_IVPU_BO_INFO:
        oss << "DRM_IOCTL_IVPU_BO_INFO, arg: "
            << struct_drm_ivpu_bo_info_to_str(static_cast<drm_ivpu_bo_info *>(arg));
        break;
    case DRM_IOCTL_IVPU_SUBMIT:
        oss << "DRM_IOCTL_IVPU_SUBMIT, arg: "
            << struct_drm_ivpu_submit_to_str(static_cast<drm_ivpu_submit *>(arg));
        break;
    case DRM_IOCTL_IVPU_BO_WAIT:
        oss << "DRM_IOCTL_IVPU_BO_WAIT, arg: "
            << struct_drm_ivpu_bo_wait_to_str(static_cast<drm_ivpu_bo_wait *>(arg));
        break;
    case DRM_IOCTL_IVPU_METRIC_STREAMER_START:
        oss << "DRM_IOCTL_IVPU_METRIC_STREAMER_START, arg: "
            << struct_drm_ivpu_metric_streamer_start_to_str(
                   static_cast<drm_ivpu_metric_streamer_start *>(arg));
        break;
    case DRM_IOCTL_IVPU_METRIC_STREAMER_STOP:
        oss << "DRM_IOCTL_IVPU_METRIC_STREAMER_STOP, arg: "
            << struct_drm_ivpu_metric_streamer_stop_to_str(
                   static_cast<drm_ivpu_metric_streamer_stop *>(arg));
        break;
    case DRM_IOCTL_IVPU_METRIC_STREAMER_GET_DATA:
        oss << "DRM_IOCTL_IVPU_METRIC_STREAMER_GET_DATA, arg: "
            << struct_drm_ivpu_metric_streamer_get_data_to_str(
                   static_cast<drm_ivpu_metric_streamer_get_data *>(arg));
        break;
    case DRM_IOCTL_IVPU_METRIC_STREAMER_GET_INFO:
        oss << "DRM_IOCTL_IVPU_METRIC_STREAMER_GET_INFO, arg: "
            << struct_drm_ivpu_metric_streamer_get_data_to_str(
                   static_cast<drm_ivpu_metric_streamer_get_data *>(arg));
        break;
    case DRM_IOCTL_IVPU_CMDQ_CREATE:
        oss << "DRM_IOCTL_IVPU_CMDQ_CREATE, arg: "
            << struct_drm_ivpu_cmdq_create_to_str(static_cast<drm_ivpu_cmdq_create *>(arg));
        break;
    case DRM_IOCTL_IVPU_CMDQ_DESTROY:
        oss << "DRM_IOCTL_IVPU_CMDQ_DESTROY, arg: "
            << struct_drm_ivpu_cmdq_destroy_to_str(static_cast<drm_ivpu_cmdq_destroy *>(arg));
        break;
    case DRM_IOCTL_IVPU_CMDQ_SUBMIT:
        oss << "DRM_IOCTL_IVPU_CMDQ_SUBMIT, arg: "
            << struct_drm_ivpu_cmdq_submit_to_str(static_cast<drm_ivpu_cmdq_submit *>(arg));
        break;
    case DRM_IOCTL_IVPU_BO_CREATE_FROM_USERPTR:
        oss << "DRM_IOCTL_IVPU_BO_CREATE_FROM_USERPTR, arg: "
            << struct_drm_ivpu_bo_create_from_userptr_to_str(
                   static_cast<drm_ivpu_bo_create_from_userptr *>(arg));
        break;
    default:
        oss << "Unknown (" << std::hex << std::showbase << request << "), arg: " << arg;
        break;
    }
    return oss.str();
}
