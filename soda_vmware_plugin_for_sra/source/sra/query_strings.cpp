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

#include "query_strings.h"
#include "sra_basic.h"
#include "sra.h"

int writeChinese(XmlWriter &writer);
int writeEnglish(XmlWriter &writer);
int write_common_error(XmlWriter &writer);

int query_strings(XmlReader &reader)
{
    XmlWriter writer;

    char xmlns[LENGTH_XMLNS] = {0};
    char output_file[LEGNTH_PATH] = {0};

    COMMLOG(OS_LOG_INFO, "%s", "query_strings begin.");
    print("%s", "query_strings begin.");
    if (!reader.get_string(XML_OUTPUTFILE, output_file, 0)){
        return ERROR_INTERNAL_PROCESS_FAIL;
    }

    if (reader.get_string(XML_REQUSET_TITLE, xmlns, 0)){
        CHECK_FALSE(writer.set_string(XML_RESPONSE_TITLE, (const char*)xmlns), ERROR_INTERNAL_PROCESS_FAIL);
    }

    char acLocale[LENGTH_COMMON] = {0};
    if (!reader.get_string(XML_QUERYSTRINGS_LOCALE, acLocale, 0)){
        (void)write_common_error(writer);
    }

    string str_locale = string(acLocale);
    (void)transform(str_locale.begin(),str_locale.end(),str_locale.begin(), ::tolower);


    if (str_locale == XML_LOCALE_CN){
        (void)writeChinese(writer);
    }
    else if (str_locale == XML_LOCALE_EN){
        (void)writeEnglish(writer);
    }
    else{
        (void)write_common_error(writer);
    }

    if(!writer.save_to_file(output_file)){
        return RETURN_ERR;
    }
    COMMLOG(OS_LOG_INFO, "%s", "query_strings end.");
    print("%s", "query_strings end.");
    return RETURN_OK;
}

int writeChinese(XmlWriter &writer)
{
    char t_buf[LENGTH_COMMON] = {0};
    wchar_t wt_buf[LENGTH_COMMON * 2] = {0};

    int index = 0;

    string description_id;
    string fixhint_id;
    string tmp_str;

    map<int, pair<string, string> >::iterator iter_err;
    map<int, pair<string, string> >::iterator iter_warn;

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

    (void)writer.set_string("/Response/Strings/@locale", "zh_CN");

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), strSraName.c_str(), sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    (void)writer.set_string("/Response/Strings/String/@id", "SRAname", index);
    index++;

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), strManuFactoryInfo.c_str(), sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));

    string stringID = strBandInfo + "Vendor";
    (void)writer.set_string("/Response/Strings/String/@id", stringID.c_str(), index);
    index++;

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), "阵列连接管理", sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    (void)writer.set_string("/Response/Strings/String/@id", "ConnectionTitle", index);
    index++;

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), "阵列连接参数管理", sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    (void)writer.set_string("/Response/Strings/String/@id", "ConnectionHint", index);
    index++;

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), "阵列SN", sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupTitleMaster", index);
    index++;

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), "输入阵列SN", sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupHintMaster", index);
    index++;

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), "用户名", sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupUsernameTitle", index);
    index++;

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), "输入阵列用户名", sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupUsernameHint", index);
    index++;

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), "密码", sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupPasswordTitle", index);
    index++;

    memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
    memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
    strncpy_s(t_buf, sizeof(t_buf), "输入阵列密码", sizeof(t_buf)-1);
    (void)SafeMultiByteToWideChar(wt_buf, t_buf);
    (void)SafeWideCharToMultiByte(t_buf, wt_buf);
    (void)writer.set_string("/Response/Strings/String", t_buf, index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupPasswordHint", index);
    index++;

    iter_err = g_map_err_cn.begin();

    while (iter_err != g_map_err_cn.end()){
        description_id = "Description" + OS_IToString(iter_err->first);
        fixhint_id = "FixHint" + OS_IToString(iter_err->first);

        tmp_str = iter_err->second.first;
        memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
        memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
        strncpy_s(t_buf, sizeof(t_buf), tmp_str.c_str(), sizeof(t_buf)-1);
        (void)SafeMultiByteToWideChar(wt_buf, t_buf);
        (void)SafeWideCharToMultiByte(t_buf, wt_buf);

        (void)writer.set_string("/Response/Strings/String", t_buf, index);
        (void)writer.set_string("/Response/Strings/String/@id", description_id.c_str(), index);
        index++;

        tmp_str = iter_err->second.second;
        memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
        memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
        strncpy_s(t_buf, sizeof(t_buf), tmp_str.c_str(), sizeof(t_buf)-1);
        (void)SafeMultiByteToWideChar(wt_buf, t_buf);
        (void)SafeWideCharToMultiByte(t_buf, wt_buf);
        (void)writer.set_string("/Response/Strings/String", t_buf, index);
        (void)writer.set_string("/Response/Strings/String/@id", fixhint_id.c_str(), index);
        index++;

        ++iter_err;
    }

    iter_warn = g_map_warn_cn.begin();

    while (iter_warn != g_map_warn_cn.end()){
        description_id = "Description" + OS_IToString(iter_warn->first);
        fixhint_id = "FixHint" + OS_IToString(iter_warn->first);

        tmp_str = iter_warn->second.first;
        memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
        memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
        strncpy_s(t_buf, sizeof(t_buf), tmp_str.c_str(), sizeof(t_buf)-1);
        (void)SafeMultiByteToWideChar(wt_buf, t_buf);
        (void)SafeWideCharToMultiByte(t_buf, wt_buf);

        (void)writer.set_string("/Response/Strings/String", t_buf, index);
        (void)writer.set_string("/Response/Strings/String/@id", description_id.c_str(), index);
        index++;

        tmp_str = iter_warn->second.second;
        memset_s(t_buf, sizeof(t_buf), 0, sizeof(t_buf));
        memset_s(wt_buf, sizeof(wt_buf), 0, sizeof(wt_buf));
        strncpy_s(t_buf, sizeof(t_buf), tmp_str.c_str(), sizeof(t_buf)-1);
        (void)SafeMultiByteToWideChar(wt_buf, t_buf);
        (void)SafeWideCharToMultiByte(t_buf, wt_buf);
        (void)writer.set_string("/Response/Strings/String", t_buf, index);
        (void)writer.set_string("/Response/Strings/String/@id", fixhint_id.c_str(), index);
        index++;

        ++iter_warn;
    }

    return RETURN_OK;
}

int writeEnglish(XmlWriter &writer)
{
    int index = 0;

    string description_id;
    string fixhint_id;
    string tmp_str;

    map<int, pair<string, string> >::iterator iter_err;
    map<int, pair<string, string> >::iterator iter_warn;

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

    (void)writer.set_string("/Response/Strings/@locale", "en");

    (void)writer.set_string("/Response/Strings/String", strSraName.c_str(), index);
    (void)writer.set_string("/Response/Strings/String/@id", "SRAname", index);
    index++;

    string stringID = strBandInfo + "Vendor";
    (void)writer.set_string("/Response/Strings/String", strManuFactoryInfo.c_str(), index);
    (void)writer.set_string("/Response/Strings/String/@id", stringID.c_str(), index);
    index++;

    (void)writer.set_string("/Response/Strings/String", "Managed Array", index);
    (void)writer.set_string("/Response/Strings/String/@id", "ConnectionTitle", index);
    index++;

    (void)writer.set_string("/Response/Strings/String", "Local Array connection parameters", index);
    (void)writer.set_string("/Response/Strings/String/@id", "ConnectionHint", index);
    index++;

    if (INBAND_TYPE == g_cli_type){
        (void)writer.set_string("/Response/Strings/String", "Array SN", index);
        (void)writer.set_string("/Response/Strings/String/@id", "GroupTitle", index);
        index++;

        (void)writer.set_string("/Response/Strings/String", "Enter Array SN", index);
        (void)writer.set_string("/Response/Strings/String/@id", "GroupHint", index);
        index++;
    }
    else{
        (void)writer.set_string("/Response/Strings/String", "Master Management IP", index);
        (void)writer.set_string("/Response/Strings/String/@id", "GroupTitleMaster", index);
        index++;

        (void)writer.set_string("/Response/Strings/String", "Enter Master Management IP", index);
        (void)writer.set_string("/Response/Strings/String/@id", "GroupHintMaster", index);
        index++;

        (void)writer.set_string("/Response/Strings/String", "Slave Management IP", index);
        (void)writer.set_string("/Response/Strings/String/@id", "GroupTitleSlave", index);
        index++;

        (void)writer.set_string("/Response/Strings/String", "Enter Slave Management IP", index);
        (void)writer.set_string("/Response/Strings/String/@id", "GroupHintSlave", index);
        index++;
    }
    

    (void)writer.set_string("/Response/Strings/String", "Username", index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupUsernameTitle", index);
    index++;

    (void)writer.set_string("/Response/Strings/String", "Enter Username for Array", index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupUsernameHint", index);
    index++;

    (void)writer.set_string("/Response/Strings/String", "Password", index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupPasswordTitle", index);
    index++;

    (void)writer.set_string("/Response/Strings/String", "Enter Password for Array", index);
    (void)writer.set_string("/Response/Strings/String/@id", "GroupPasswordHint", index);
    index++;

    iter_err = g_map_err_en.begin();

    while (iter_err != g_map_err_en.end()){
        description_id = "Description" + OS_IToString(iter_err->first);
        fixhint_id = "FixHint" + OS_IToString(iter_err->first);

        tmp_str = iter_err->second.first;
        (void)writer.set_string("/Response/Strings/String", tmp_str.c_str(), index);
        (void)writer.set_string("/Response/Strings/String/@id", description_id.c_str(), index);
        index++;

        tmp_str = iter_err->second.second;
        (void)writer.set_string("/Response/Strings/String", tmp_str.c_str(), index);
        (void)writer.set_string("/Response/Strings/String/@id", fixhint_id.c_str(), index);
        index++;

        ++iter_err;
    }

    iter_warn = g_map_warn_en.begin();

    while (iter_warn != g_map_warn_en.end()){
        description_id = "Description" + OS_IToString(iter_warn->first);
        fixhint_id = "FixHint" + OS_IToString(iter_warn->first);

        tmp_str = iter_warn->second.first;
        (void)writer.set_string("/Response/Strings/String", tmp_str.c_str(), index);
        (void)writer.set_string("/Response/Strings/String/@id", description_id.c_str(), index);
        index++;

        tmp_str = iter_warn->second.second;
        (void)writer.set_string("/Response/Strings/String", tmp_str.c_str(), index);
        (void)writer.set_string("/Response/Strings/String/@id", fixhint_id.c_str(), index);
        index++;

        ++iter_warn;
    }

    return RETURN_OK;
}

int write_common_error(XmlWriter &writer)
{
    (void)writer.set_int(XML_RESPONSE_ERROR, ERROR_LOCALE_NOT_SUPPORTED);

    return RETURN_OK;
}
