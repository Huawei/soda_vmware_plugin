// Copyright 2019 The OpenSDS Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may
// not use this file except in compliance with the License. You may obtain
// a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

#ifndef XMLCOMMON_H
#define XMLCOMMON_H

#include <stddef.h>

#define CHECK_NULL(ptr, ret) \
{ \
        if ( NULL == (ptr) ) \
        { \
            return ret; \
        } \
}

#define CHECK_FALSE(ptr, ret) \
{ \
        if ( false == (ptr) ) \
        { \
            return ret; \
        } \
}

#define CHECK_MORETHAN(ptr1, ptr2, ret) \
{ \
        if ( (ptr1) > (ptr2) ) \
        { \
            return ret; \
        } \
}

#endif

