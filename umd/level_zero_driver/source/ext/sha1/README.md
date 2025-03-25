<!-- Copyright 2022-2025 Intel Corporation.

 This software and the related documents are Intel copyrighted materials, and
 your use of them is governed by the express license under which they were
 provided to you ("License"). Unless the License provides otherwise, you may
 not use, modify, copy, publish, distribute, disclose or transmit this
 software or the related documents without Intel's prior written permission.

 This software and the related documents are provided "as is", with no express
 or implied warranties, other than those that are expressly stated in
 the License. -->

This SHA1 implementation comes from OpenBSD repository:

https://github.com/openbsd/src/blob/e516815e691450dd88a57920f79f884547e9592c/include/sha1.h
https://github.com/openbsd/src/blob/e516815e691450dd88a57920f79f884547e9592c/lib/libc/hash/sha1.c

with a couple of modifications:
- remove unused function declarations from header
- remove usage of `DEF_WEAK` macro
- use `memset_explicit` on Android
- rename `u_int` like types to `uint` types from stdint.h
