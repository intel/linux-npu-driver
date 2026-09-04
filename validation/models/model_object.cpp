/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "model_object.h"

extern const uint8_t _binary_add_abc_add_abc_bin_start[];
extern const uint8_t _binary_add_abc_add_abc_bin_end[];
extern const uint8_t _binary_add_abc_add_abc_xml_start[];
extern const uint8_t _binary_add_abc_add_abc_xml_end[];
extern const uint8_t _binary_add_abc_add_abc_conf_start[];
extern const uint8_t _binary_add_abc_add_abc_conf_end[];

const uint8_t *add_abc_bin = _binary_add_abc_add_abc_bin_start;
const uint8_t *add_abc_xml = _binary_add_abc_add_abc_xml_start;
const uint8_t *add_abc_conf = _binary_add_abc_add_abc_conf_start;
const uint64_t add_abc_bin_len =
    _binary_add_abc_add_abc_bin_end - _binary_add_abc_add_abc_bin_start;
const uint64_t add_abc_xml_len =
    _binary_add_abc_add_abc_xml_end - _binary_add_abc_add_abc_xml_start;
const uint64_t add_abc_conf_len =
    _binary_add_abc_add_abc_conf_end - _binary_add_abc_add_abc_conf_start;

extern const uint8_t _binary_mul_add_mul_add_bin_start[];
extern const uint8_t _binary_mul_add_mul_add_bin_end[];
extern const uint8_t _binary_mul_add_mul_add_xml_start[];
extern const uint8_t _binary_mul_add_mul_add_xml_end[];

const uint8_t *mul_add_bin = _binary_mul_add_mul_add_bin_start;
const uint8_t *mul_add_xml = _binary_mul_add_mul_add_xml_start;
const uint8_t *mul_add_conf = nullptr;
const uint64_t mul_add_bin_len =
    _binary_mul_add_mul_add_bin_end - _binary_mul_add_mul_add_bin_start;
const uint64_t mul_add_xml_len =
    _binary_mul_add_mul_add_xml_end - _binary_mul_add_mul_add_xml_start;
const uint64_t mul_add_conf_len = 0;

const std::array<internal_model_t, 2> embeddedModel = {{{"Internal_add_abc",
                                                         add_abc_xml,
                                                         add_abc_bin,
                                                         add_abc_conf,
                                                         add_abc_xml_len,
                                                         add_abc_bin_len,
                                                         add_abc_conf_len},
                                                        {"Internal_mul_add",
                                                         mul_add_xml,
                                                         mul_add_bin,
                                                         mul_add_conf,
                                                         mul_add_xml_len,
                                                         mul_add_bin_len,
                                                         mul_add_conf_len}}};
