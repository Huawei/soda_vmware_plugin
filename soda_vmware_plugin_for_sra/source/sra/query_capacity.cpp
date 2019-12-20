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

#include "query_capacity.h"
#include "sra_basic.h"
#include "sra.h"

int query_capacity(XmlReader &reader)
{
    XmlWriter writer;

    char xmlns[LENGTH_XMLNS] = {0};
    char output_file[LEGNTH_PATH] = {0};

    COMMLOG(OS_LOG_INFO, "%s", "query_capacity begin.");
    print("%s", "query_capacity begin.");
    if (!reader.get_string(XML_OUTPUTFILE, output_file, 0)){
        return ERROR_INTERNAL_PROCESS_FAIL;
    }

    if (reader.get_string(XML_REQUSET_TITLE, xmlns, 0)){
        CHECK_FALSE(writer.set_string(XML_RESPONSE_TITLE, (const char*)xmlns), ERROR_INTERNAL_PROCESS_FAIL);
    }
    string strBandInfo;
    string strManuFactoryInfo;
    string strProductModel;

    if (!g_bFusionStorage){
        CISMConfig oConfig;
        if (oConfig.open("config",".txt") != false){
            if(oConfig.getStringValue("BandInfo", strBandInfo) == false){
                strBandInfo = "OceanStor";
            }

            if(oConfig.getStringValue("ManuFactoryInfo", strManuFactoryInfo) == false){
                strManuFactoryInfo = "******";
            }

            if(oConfig.getStringValue("ProductModel", strProductModel) == false){
                strProductModel = "S2600T/S5500T/S5600T/S5800T/S6800T V200R002,18500/18800/18800F V100R001,5300/5500/5600/5800/6800 V3 V300R001 V300R002 V300R003 V300R006,";
                strProductModel = strProductModel + "18500/18800 V3 V300R003 V300R006,2200/2600 V3 V300R005 V300R006,2100 V3 V300R006,2600F/5300F/5500F/5600F/5800F/6800F/18500F/18800F V3 V300R006,";
                strProductModel = strProductModel + "Dorado 5000/6000V3 V300R001, 5300/5500/5600/5800/6800/5300F/5500F/5600F/5800F/6800F V5 V500R007, 18500/18800/18500F/18800F V5 V500R007";
            }
        }
        else{
            COMMLOG(OS_LOG_ERROR, "open config file failed!");
        }
    }
    else{
        strBandInfo = "FusionStorage";
        strManuFactoryInfo = "******";
        strProductModel = "8.0";
    }

   
    string Softwarename = strBandInfo + " DeviceManager";
    (void)writer.set_string("/Response/AdapterCapabilities/ReplicationSoftwares/ReplicationSoftware/Name", Softwarename.c_str());//"OceanStor DeviceManager");
    (void)writer.set_string("/Response/AdapterCapabilities/ReplicationSoftwares/ReplicationSoftware/Version", " ");
    vector<string> vecinfo;
    string name = "";
    CISMConfig::split(strProductModel,",",vecinfo);
    for (unsigned i = 0; i < vecinfo.size(); i++){
        if (i != 0){
            name = name + "              " +strBandInfo + " " + vecinfo[i] + '\n';
        }
        else{
            name = name + strBandInfo + " " + vecinfo[i] + '\n';
        }
    }
    string stringID = strBandInfo + "Vendor";
    (void)writer.set_string("/Response/AdapterCapabilities/ArrayModels/ArrayModel/Name", name.c_str());
    (void)writer.set_string("/Response/AdapterCapabilities/ArrayModels/ArrayModel/Vendor", strManuFactoryInfo.c_str());
    (void)writer.set_string("/Response/AdapterCapabilities/ArrayModels/ArrayModel/Vendor/@stringId", stringID.c_str());

    (void)writer.set_string("/Response/AdapterCapabilities/Features/DynamicAccessRestriction","");
    
    if (g_bstretch){
        (void)writer.set_string("/Response/AdapterCapabilities/Features/StretchedStorage","");
    }
    (void)writer.set_string("/Response/AdapterCapabilities/Features/DeviceIdentification", "wwn");
    (void)writer.set_string("/Response/AdapterCapabilities/Features/Protocols/Protocol", "iSCSI", 0);
    (void)writer.set_string("/Response/AdapterCapabilities/Features/Protocols/Protocol", "FC", 1);
    if (g_bnfs){
        (void)writer.set_string("/Response/AdapterCapabilities/Features/Protocols/Protocol", "NFS", 2);
    }


    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "queryStrings", 0);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/Locales/Locale/@id/", "en", 0);

    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "queryInfo", 1);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "queryErrorDefinitions", 2);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "queryCapabilities", 3);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "queryConnectionParameters", 4);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "discoverArrays", 5);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "discoverDevices", 6);

    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command", "", 7);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name", "syncOnce", 7);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/ExecutionLocation", "source");

    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "querySyncStatus", 8);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "testFailoverStart", 9);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "testFailoverStop", 10);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "failover", 11);

    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command", "", 12);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name", "restoreReplication");
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/ExecutionLocation", "source");

    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command", "", 13);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name", "reverseReplication");
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/ExecutionLocation", "source");

    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "prepareFailover", 14);
    (void)writer.set_string("/Response/AdapterCapabilities/Commands/Command/@name/", "prepareReverseReplication", 15);

    if(!writer.save_to_file(output_file)){
        return RETURN_ERR;
    }

    COMMLOG(OS_LOG_INFO, "%s", "query_capacity end.");
    print("%s", "query_capacity end.");
    return RETURN_OK;
}

