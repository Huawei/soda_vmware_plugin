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

#include "xmlserializable.h"
#include "xmlcommon.h"
#include <stdio.h>

int get_realpathdeepth(const char * path)
{
    int deepth = 0;

    CHECK_NULL(path, 0);

    while (*path)
    {
        if ('/' == *path)
        {
            deepth++;
        }

        path++;
    }

    return deepth;
}

int get_pathdeepth(const char * path)
{
    int deepth = 0;
    bool end_flag = false;
    bool ret = false;
     char name_array[10][NAMELEN] = {0};
     const char * path_temp = path;

    CHECK_NULL(path, 0);

    while (*path)
    {
        if ('/' == *path)
        {
            deepth++;
        }

        path++;
    }

    ret = get_allelemname_bypath(path_temp, name_array, end_flag);
    if(false == ret)
    {
        return -1;
    }

    if(true == end_flag)
    {
        deepth -= 1;
    }

    return deepth;
}

bool get_allelemname_bypath(const char *path, char name_array[][NAMELEN], bool &end_flag)
{
    int i = 0;
    int j = 0;
    bool flag = false;
    int length = 0;

    length =  get_realpathdeepth(path);

    CHECK_NULL(path, false);
    CHECK_NULL(name_array, false);

    while (*path)
    {
        if (!flag && ('/' == *path))
        {
            flag = true;
            path++;
            continue;
        }

        if ('/' == *path)
        {
            name_array[i++][j] = '\0';
            if('\0' == *(path +1))
            {
               i--;
               break;
            }

            j = 0;
        }
        else
        {
            name_array[i][j++] = *path;
        }

        path++;
    }

    name_array[i][j] = '\0';
    if(i != (length -1))
    {
        end_flag = true;
        return true;
    }
    end_flag = false;

    return true;

}




