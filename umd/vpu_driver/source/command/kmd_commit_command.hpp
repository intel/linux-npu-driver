/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <firmware/vpu_jsm_job_cmd_api.h>

namespace VPU {

/**
 KMD commiting commands template.
*/
template <typename T>
struct KMDCommitCommand {
    /**
      Returns KMD commiting command's size.
     */
    inline size_t getKMDCommitSize() const { return sizeof(cmd); }

    /**
      Return KMD commiting byte stream.
     */
    inline const uint8_t *getKMDCommitStream() const {
        return reinterpret_cast<const uint8_t *>(&cmd);
    }

    /**
      Return KMD command type.
     */
    inline vpu_cmd_type getKMDCommandType() const {
        return static_cast<vpu_cmd_type>(cmd.header.type);
    }

    T cmd = {};
};

} // namespace VPU
