/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "api/vpu_jsm_job_cmd_api.h"
#include "vpu_driver/source/utilities/log.hpp"

#include <any>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace VPU {

class VPUCommandBuffer;
class VPUDeviceContext;
class VPUBufferObject;

struct VPUDescriptor {
    std::vector<char> data = {};
    uint64_t *commandOffset = nullptr;
    uint32_t numDescriptors = 0;
};

class VPUCommand {
  public:
    enum class ScheduleType {
        /* Generic command that is added to command buffer */
        Generic,
        /**
         * Synchronize command ends the command buffer. All the following commands are added to new
         * command buffer
         */
        Synchronize,
    };

    VPUCommand(ScheduleType type = ScheduleType::Generic);

    VPUCommand(VPUCommand const &) = delete;
    VPUCommand &operator=(VPUCommand const &) = delete;

    virtual ~VPUCommand(){};

    size_t getCommitSize() const {
        const vpu_cmd_header_t *hdr = this->getHeader();
        if (!hdr)
            return 0;
        return hdr->size;
    }

    const uint8_t *getCommitStream() const {
        return reinterpret_cast<const uint8_t *>(this->getHeader());
    }

    vpu_cmd_type getCommandType() const {
        const vpu_cmd_header_t *hdr = this->getHeader();
        if (!hdr)
            return VPU_CMD_UNKNOWN;
        return static_cast<vpu_cmd_type>(hdr->type);
    }

    size_t getDescriptorSize() const { return descriptor ? descriptor->data.size() : 0; }

    const std::vector<std::shared_ptr<VPUBufferObject>> &getAssociateBufferObjects() const {
        return bufferObjects;
    }

    inline bool isSynchronizeCommand() const { return sType == ScheduleType::Synchronize; }

    bool copyDescriptor(void **desc, std::shared_ptr<VPUBufferObject> bo);

    using ArgumentUpdatesMap =
        std::unordered_map<uint32_t, const void *>; // key - argument index
                                                    // value - new pointer for the argument
    virtual bool setUpdates(const ArgumentUpdatesMap &updatesMap) {
        LOG_E("Command with type %#x does not support changing arguments", getCommandType());
        return false;
    }

    virtual bool update(VPUCommandBuffer *commandBuffer) {
        LOG_E("Command with type %#x does not support changing arguments", getCommandType());
        return false;
    }

    bool needsUpdate() const { return cmdNeedsUpdate; }

  protected:
    bool appendAssociateBufferObject(VPUDeviceContext *ctx, const void *assocPtr);
    void appendAssociateBufferObject(
        const std::vector<std::shared_ptr<VPUBufferObject>> &bufferObjectsInput);
    void appendAssociateBufferObject(VPUDeviceContext *ctx, VPUBufferObject *bo);
    void appendAssociateBufferObject(const std::shared_ptr<VPUBufferObject> bo);
    void eraseAssociatedBufferObjects(size_t pos);

    void setDescriptor(VPUDescriptor &&d) { descriptor = std::move(d); }
    virtual const vpu_cmd_header_t *getHeader() const { return nullptr; }

    std::any command = {};

    bool cmdNeedsUpdate = false;

  private:
    ScheduleType sType;
    std::vector<std::shared_ptr<VPUBufferObject>> bufferObjects = {};
    std::optional<VPUDescriptor> descriptor = {};
};
} // namespace VPU
