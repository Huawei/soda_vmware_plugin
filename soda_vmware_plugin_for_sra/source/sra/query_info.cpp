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

#include "query_info.h"
#include "sra_basic.h"
#include "sra.h"

int query_info(XmlReader &reader)
{
    XmlWriter writer;

    char xmlns[LENGTH_XMLNS] = {0};
    char output_file[LEGNTH_PATH] = {0};

    COMMLOG(OS_LOG_INFO, "%s", "query_info begin.");
    print( "%s", "query_info begin.");
    if (!reader.get_string(XML_OUTPUTFILE, output_file, 0)){
        return ERROR_INTERNAL_PROCESS_FAIL;
    }

    if (reader.get_string(XML_REQUSET_TITLE, xmlns, 0)){
        CHECK_FALSE(writer.set_string(XML_RESPONSE_TITLE, (const char*)xmlns), ERROR_INTERNAL_PROCESS_FAIL);
    }
    string strBandInfo;
    string strManuFactoryInfo;
    string strSraName;
    CISMConfig oConfig;
    if (oConfig.open("config",".txt") != false){
        if(oConfig.getStringValue("BandInfo", strBandInfo) == false){
            strBandInfo = "OceanStor";
        }
        if(oConfig.getStringValue("ManuFactoryInfo", strManuFactoryInfo) == false){
            strManuFactoryInfo = "******";
        }
        if(oConfig.getStringValue("SraName", strSraName) == false){
            strSraName = "OpenSDS_Storage_SRA";
        }
    }

    (void)writer.set_string("/Response/AdapterInfo/Name", strSraName.c_str());
    (void)writer.set_string("/Response/AdapterInfo/Name/@stringId", "SRAname");
    (void)writer.set_string("/Response/AdapterInfo/Version", SRA_VERSION);
    (void)writer.set_string("/Response/AdapterInfo/Vendor", strManuFactoryInfo.c_str());
    string stringID = strBandInfo + "Vendor";
    (void)writer.set_string("/Response/AdapterInfo/Vendor/@stringId", stringID.c_str());
    (void)writer.set_string("/Response/AdapterInfo/Uuid", "123-456-789");

    if(!writer.save_to_file(output_file)){
        return RETURN_ERR;
    }

    COMMLOG(OS_LOG_INFO, "%s", "query_info end.");
    print("%s", "query_info end.");
    return RETURN_OK;
}
