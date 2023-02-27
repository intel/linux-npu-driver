/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <optional>
#include <uapi/drm/ivpu_accel.h>
#include <any>

namespace VPU {

class VPUDeviceContext;

struct VPUDescriptor {
    std::vector<char> data = {};
    uint64_t *commandOffset = nullptr;
};

class VPUCommand {
  public:
    enum class EngineSupport {
        /* Compute command can be only run on compute engine */
        Compute,
        /* Copy command can be only run on copy engine */
        Copy,
        /**
         * Forward and Backward commands can be run on both engine. Those commands are
         * added to copy or compute engine based on logic in command list implementation.
         * Forward commands are added to the next command list when there is a switch between
         * engines. Backward commands are added to the previous command list when there is a
         * switch between engines. If there is no switch between engines then command is added to
         * latest used engine.
         */
        Forward,
        Backward
    };

    /**
     * Default commands shall usually be backward following unless specified.
     */
    VPUCommand(EngineSupport type = EngineSupport::Backward);

    // Block copy constructors.
    VPUCommand(VPUCommand const &) = delete;
    VPUCommand &operator=(VPUCommand const &) = delete;

    virtual ~VPUCommand(){};

    /**
     * Returns size of the actual commit struct.
     */
    virtual size_t getCommitSize() const {
        const vpu_cmd_header_t *hdr = this->getHeader();
        if (!hdr)
            return 0;
        return hdr->size;
    }

    /**
     * Return the commiting struct data in byte stream.
     */
    virtual const uint8_t *getCommitStream() const {
        return reinterpret_cast<const uint8_t *>(this->getHeader());
    }
    /**
     * Return the command's type
     */
    virtual vpu_cmd_type getCommandType() const {
        const vpu_cmd_header_t *hdr = this->getHeader();
        if (!hdr)
            return VPU_CMD_UNKNOWN;
        return static_cast<vpu_cmd_type>(hdr->type);
    }

    /**
     * Return size of descriptor
     */
    size_t getDescriptorSize() const { return descriptor ? descriptor->data.size() : 0; }

    /**
     * Returns private member variable bufferObjects: contains associated VPUBufferObject pointers
     * related to commandlist.
     * @return vector of VPUBufferObject pointers
     */
    const std::vector<VPUBufferObject *> &getAssociateBufferObjects() const {
        return bufferObjects;
    }

    /**
     * Return true if the command can be run on copy engine
     */
    inline bool isCopyCommand() const { return engineSupport == EngineSupport::Copy; }

    /**
     * Return true if the command can be run on compute engine
     */
    inline bool isComputeCommand() const { return engineSupport == EngineSupport::Compute; }

    /**
     * Set the engineSupport of the command to compute engine
     */
    inline void setComputeCommand() { engineSupport = EngineSupport::Compute; }

    /**
     * Return true if the command is backward following command
     */
    inline bool isBackwardCommand() const { return engineSupport == EngineSupport::Backward; }

    /**
     * Return true if the command is forward following command
     */
    inline bool isForwardCommand() const { return engineSupport == EngineSupport::Forward; }

    /**
     * Return true if this command can be run on copy and compute engine
     */
    inline bool isCommandAgnostic() const { return isForwardCommand() || isBackwardCommand(); }

    /**
     * Update offsets and descriptor addresses as required by specific commands using heap base
     * addresses from command buffer
     */
    static void updateResourceDescriptorTable(void **desc,
                                              vpu_desc_table_entry_type type,
                                              const uint64_t address,
                                              const size_t size);

    static void updateResourceDescriptorTable(void **desc,
                                              vpu_desc_table_entry_type type,
                                              const VPUBufferObject *bo);

    static void updateResourceDescriptorTable(void **desc,
                                              vpu_desc_table_entry_type type,
                                              const std::vector<uint64_t> &addrs,
                                              const std::vector<uint32_t> &size);

    static void updateResourceDescriptorTable(void **desc,
                                              vpu_desc_table_entry_type type,
                                              const std::vector<uint64_t> &addrs,
                                              size_t size);

    bool copyDescriptor(VPUDeviceContext *ctx, void **desc);

  protected:
    /**
     * Add associated VPUBufferObject to vector bufferObjects.
     */
    bool appendAssociateBufferObject(VPUDeviceContext *ctx, const void *assocPtr);
    /**
     * Add associated VPUBufferObject to vector bufferObjects.
     */
    void appendAssociateBufferObject(const std::vector<VPUBufferObject *> &bufferObjectsInput);
    /**
     * Add associated VPUBufferObject to vector bufferObjects.
     */
    void appendAssociateBufferObject(VPUBufferObject *bo);
    void setDescriptor(VPUDescriptor &&d) { descriptor = std::move(d); }
    virtual const vpu_cmd_header_t *getHeader() const { return nullptr; }

    std::any command = {};

  private:
    EngineSupport engineSupport;
    std::vector<VPUBufferObject *> bufferObjects = {};
    std::optional<VPUDescriptor> descriptor = {};
};
} // namespace VPU
