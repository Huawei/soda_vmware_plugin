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

/*===========================================================================
  File Name    : RsltXmlParser.h
  Author       : 
  Version      :
  DateTime     :
  Others       :
  Description  : The head file parse inbandAgent's tempary result XML file.
  Function List:
  Method List  :
  History      :
  <author>  <time>   <version >   <desc>
===========================================================================*/
#ifndef _RSLT_PARSER_H
#define _RSLT_PARSER_H

#include "../common.h"

#define MIN_TMP_BUFFER  100
#define MAX_TMP_BUFFER 1000

#define CONDITION_TRUE_RTN_FAIL(v_Contion) \
    do \
    {  \
        if (v_Contion) \
        { \
            return RETURN_ERR; \
        } \
    }while (0)

class CRsltXmlParser
{
public:
    CRsltXmlParser();
    CRsltXmlParser(const char *pszRstlFilePath);
    ~CRsltXmlParser();

    string GetRstlFilePath() const;
    int SetRstlFilePath(const char *pszRstlFilePath);
    int GetStatus();
    int GetValues(const string &rstrSrcName, const string &rstrSrcValue, 
                         const string &rstrDstName, list<string> *plstDstValue);
    int GetValues(const string &rstrSrcName, const string &rstrSrcValue, 
        const string &rstrDstName, string& dstValue);
    void GetImgStatus() {(void)_GetStatus();};

    int GetDescription(string& desc);

private:
    int _InitRstlFile();
    int _ParseRecordName();
    int _ParseRecordValue();
    int _GetStatus();
    int _StrToken();

    int m_iStatus;          //Status in the result file
    int m_iSrcPos;          //Source string location
    int m_iDstPos;          //Target string position
    string m_strRstlFilePath;      //Result file path
    string m_strSrcName;           //Parsing the source name
    string m_strSrcValue;          //Parsing the source value
    string m_strDstName;           //Analyze the target name
    list<string> *m_plstDstValue;  //Store the parsed target value list
    XmlReader m_objXmlReader;
  
};

#endif

