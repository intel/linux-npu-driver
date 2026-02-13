<!-- Copyright (C) 2026 Intel Corporation
     SPDX-License-Identifier: MIT -->

This CityHash implementation comes from https://github.com/google/cityhash
repository(commit f5dc54147fcce12cefd16548c8e760d68ac04226):

https://github.com/google/cityhash/blob/master/src/city.cc
https://github.com/google/cityhash/blob/master/src/city.h
https://github.com/google/cityhash/blob/master/src/citycrc.h

Some modifications were done:
- removed "config.h" file include
- added directives to disable clang and iwyu checks
