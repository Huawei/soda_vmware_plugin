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

#ifndef _SRA_H
#define _SRA_H

#include <iostream>
#include <list>
#include <sstream>

#include "CmdAdapter.h"
#include "../TlvCom/msg_package.h"
#include "../TlvCom/communication.h"

#ifndef WIN32
#include <stdio.h>
#include <unistd.h>
#endif

typedef int (*SRA_ENTRY)(const XmlReader &reader);

extern "C"
{
    int dispatch(XmlReader &reader);

    void register_fun(string key, SRA_ENTRY value);

    void sra_init_reg_function();

    void sra_init_log(char* program_name);

    int send_error_info(XmlReader &reader, int error_code);

    void sra_init_err_code();
}

class  CISMConfig
{
public:
    CISMConfig() {}
    ~CISMConfig() {}

    bool open(const char* pFileName, const char* pSuffix = ".cfg", bool bCompletePath = false);
    bool getIntValue(const char* pKey, int& iValue)
    {
        std::string strKey(pKey);
        strKey = strKey + "=";

        std::string strValue;
        if(true == find(strKey, strValue)){
            iValue = ACE_OS::atoi(strValue.c_str());
            return true;
        }
        return false;
    }

    bool getStringValue(const char* pKey, std::string& strValue)
    {
        std::string strKey(pKey);
        strKey = strKey + "=";

        return find(strKey, strValue);
    }

    static std::string getHomePath();
    static void split( const std::string & str,const std::string & delm_str,std::vector<std::string> & parts );

    bool modify(const std::string & strKey, const std::string &strVale, 
            const char* pFileName, const char* pSuffix, bool bCompletePath);
private:
    bool find(const std::string& strKey, std::string& str, size_t offset = 0);
private:
    std::string m_oFile;
};

#endif
