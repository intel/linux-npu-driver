<!-- Copyright (C) 2022-2025 Intel Corporation
     SPDX-License-Identifier: MIT -->

This SHA1 implementation comes from OpenBSD repository:

https://github.com/openbsd/src/blob/e516815e691450dd88a57920f79f884547e9592c/include/sha1.h
https://github.com/openbsd/src/blob/e516815e691450dd88a57920f79f884547e9592c/lib/libc/hash/sha1.c

with a couple of modifications:
- remove unused function declarations from header
- remove usage of `DEF_WEAK` macro
- use `memset_explicit` on Android
- rename `u_int` like types to `uint` types from stdint.h
