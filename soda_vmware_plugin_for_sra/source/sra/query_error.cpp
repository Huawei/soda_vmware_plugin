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

#include "../common/error_code.h"
#include "../common/xml_node/commond_info.h"
#include "sra_basic.h"

int query_error(XmlReader &reader)
{
    XmlWriter writer;

    string description_id;
    string fixhint_id;

    map<int, pair<string, string> >::iterator iter_err;
    map<int, pair<string, string> >::iterator iter_warn;

    ErrorDefinitions err_definitions;
    WarningDefinitions warn_definitions;

    char xmlns[LENGTH_XMLNS] = {0};
    char output_file[LEGNTH_PATH] = {0};

    COMMLOG(OS_LOG_INFO, "%s", "query_error begin.");
    print("%s", "query_error begin.");

    if (!reader.get_string(XML_OUTPUTFILE, output_file, 0)){
        return ERROR_INTERNAL_PROCESS_FAIL;
    }

    if (reader.get_string(XML_REQUSET_TITLE, xmlns, 0)){
        CHECK_FALSE(writer.set_string(XML_RESPONSE_TITLE, (const char*)xmlns), ERROR_INTERNAL_PROCESS_FAIL);
    }

    iter_err = g_map_err_en.begin();

    while (iter_err != g_map_err_en.end()){
        description_id = "Description" + OS_IToString(iter_err->first);
        fixhint_id = "FixHint" + OS_IToString(iter_err->first);

        ErrorDefinition error;
        error.code = OS_IToString(iter_err->first);
        error.desc_id = description_id;
        error.desc = iter_err->second.first;
        error.hint_id = fixhint_id;
        error.hint = iter_err->second.second;
        
        err_definitions.lst_err.push_back(error);

        ++iter_err;
    }

    iter_warn = g_map_warn_en.begin();

    while (iter_warn != g_map_warn_en.end()){
        description_id = "Description" + OS_IToString(iter_warn->first);
        fixhint_id = "FixHint" + OS_IToString(iter_warn->first);

        WarningDefinition warn;
        warn.code = OS_IToString(iter_warn->first);
        warn.desc_id = description_id;
        warn.desc = iter_warn->second.first;
        warn.hint_id = fixhint_id;
        warn.hint = iter_warn->second.second;
        
        warn_definitions.lst_warn.push_back(warn);

        ++iter_warn;
    }

    (void)writer.set_xml("/Response", &err_definitions);
    (void)writer.set_xml("/Response", &warn_definitions);

    if(!writer.save_to_file(output_file)){
        return RETURN_ERR;
    }

    COMMLOG(OS_LOG_INFO, "%s", "query_error end.");
    print("%s", "query_error end.");

    return RETURN_OK;
}

