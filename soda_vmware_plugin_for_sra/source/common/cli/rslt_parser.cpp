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
  File Name    : RsltXmlParser.cpp
  Author       : 
  Version      :
  DateTime     : 2009.09.19
  Others       :
  Description  : The main function parse inbandAgent's tempary result XML file.
  Function List:
  Method List  :
  History      :
  <author>  <time>   <version >   <desc>
===========================================================================*/

#include "rslt_parser.h"

/*------------------------------------------------------------
Method       : CRsltXmlParser::CRsltXmlParser()
Description  : Class CRsltXmlParser default constructor.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
CRsltXmlParser::CRsltXmlParser()
{
    m_iStatus = 0;
    m_iSrcPos = -1;
    m_iDstPos = -1;
    m_plstDstValue = NULL;
}

/*------------------------------------------------------------
Method       : CRsltXmlParser::CRsltXmlParser()
Description  : Class CRsltXmlParser constructor.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
CRsltXmlParser::CRsltXmlParser(const char *pszRstlFilePath)
{
    m_iStatus = 0;
    m_iSrcPos = -1;
    m_iDstPos = -1;
    m_plstDstValue = NULL;

    m_strRstlFilePath = string(pszRstlFilePath);
}

/*------------------------------------------------------------
Method       : CRsltXmlParser::~CRsltXmlParser()
Description  : Class CRsltXmlParser deconstructor.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
CRsltXmlParser::~CRsltXmlParser()
{
    m_plstDstValue = NULL;
}

/*------------------------------------------------------------
Method       : int CRsltXmlParser::GetRstlFilePath()
Description  : Get tempary XML result file's path.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
string CRsltXmlParser::GetRstlFilePath() const
{
    return m_strRstlFilePath;
}

/*------------------------------------------------------------
Method       : int CRsltXmlParser::SetRstlFilePath()
Description  : Set tempary XML result file's path.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
int CRsltXmlParser::SetRstlFilePath(const char *pszRstlFilePath)
{
    if ((NULL == pszRstlFilePath) || (0 == strlen(pszRstlFilePath))){
        return RETURN_ERR;
    }

    m_strRstlFilePath = string(pszRstlFilePath);
    return RETURN_OK;
}

/*------------------------------------------------------------
Method       : int CRsltXmlParser::GetValues()
Description  : Get the value that you need.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
int CRsltXmlParser::GetValues(const string &rstrSrcName, 
                                     const string &rstrSrcValue, 
                                     const string &rstrDstName, 
                                     list<string> *plstDstValue)
                                     {
    int iNum = 0;

    if (NULL == plstDstValue){
        return RETURN_ERR;
    }

    plstDstValue->clear();

    m_strSrcName   = rstrSrcName;
    m_strSrcValue  = rstrSrcValue;
    m_strDstName   = rstrDstName;
    m_plstDstValue = plstDstValue;

    m_iStatus = 0;
    m_iSrcPos = -1;
    m_iDstPos = -1;

    if (RETURN_ERR == _GetStatus()){
        return RETURN_ERR;
    }

    iNum = m_objXmlReader.get_count("/Result/RecordName");
    if (0 == iNum){
        return RETURN_OK;
    }

    if (_ParseRecordName() != RETURN_OK){
        return RETURN_ERR;
    }

    if (_ParseRecordValue() != RETURN_OK){
        return RETURN_ERR;
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Method       : int CRsltXmlParser::GetValues()
Description  : Get the value that you need. 
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
int CRsltXmlParser::GetValues(const string &rstrSrcName, const string &rstrSrcValue, 
              const string &rstrDstName, string& dstValue)
              {
    list<string> lst_results;

    if (RETURN_OK != GetValues(rstrSrcName, rstrSrcValue, rstrDstName, &lst_results)){
        return RETURN_ERR;
    }

    if (lst_results.empty()){
        return RETURN_ERR;
    }

    dstValue = lst_results.front();

    return RETURN_OK;

}

/*------------------------------------------------------------
Method       : int CRsltXmlParser::_InitRstlFile()
Description  : Init tempary XML result file.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
int CRsltXmlParser::_InitRstlFile()
{
    if (!OS_FileExists((char *)m_strRstlFilePath.c_str())){
        return RETURN_ERR;
    }

    if (!m_objXmlReader.load_from_filename((char *)m_strRstlFilePath.c_str(),TIXML_DEFAULT_ENCODING )){
        return RETURN_ERR;
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Method       : int CRsltXmlParser::_GetStatus()
Description  : Get tempary XML result file's status.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : 1)RETURN_OK
               2)RETURN_ERR
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
int CRsltXmlParser::_GetStatus()
{
    char acErrorDesc[UNIT_K] = {0};

    if (_InitRstlFile() != RETURN_OK){
        return RETURN_ERR;
    }

    if (!m_objXmlReader.get_int("/Result/Status", m_iStatus, 0)){
        return RETURN_ERR;
    }

    if (0 != m_iStatus){
        if (0 ==  m_objXmlReader.get_string("/Result/Desc", acErrorDesc, 0)){
            return RETURN_ERR;
        }

        return RETURN_ERR;
    }

    return RETURN_OK;
}

/*------------------------------------------------------------
Method       : int CRsltXmlParser::GetStatus()
Description  : Get tempary XML result file's status.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : 1)m_iStatus
               2)RETURN_ERR
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
int CRsltXmlParser::GetStatus()
{
    if (_InitRstlFile() != RETURN_OK){
        return RETURN_ERR;
    }

    if (!m_objXmlReader.get_int("/Result/Status", m_iStatus, 0)){
        return RETURN_ERR;
    }

    return m_iStatus;
}

int CRsltXmlParser::GetDescription(string& desc)
{
    char ac_desc[UNIT_K] = {0};

    if (_InitRstlFile() != RETURN_OK){
        return RETURN_ERR;
    }

    if (!m_objXmlReader.get_string("/Result/Desc", ac_desc, 0)){
        return RETURN_ERR;
    }

    desc = ac_desc;

    return RETURN_OK;
};

/*------------------------------------------------------------
Method       : int CRsltXmlParser::_ParseRecordName()
Description  : Parse "/Result/RecordName" in tempary XML result file.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
int CRsltXmlParser::_ParseRecordName()
{
    char acTmpRecordName[MAX_TMP_BUFFER];
    int iTmpSrcPos = 0;
    int iTmpDstPos = 0;
    int iTmpPos = -1;
    char cSepChr = ',';
    char acTmpStr[MAX_TMP_BUFFER];

    memset_s(acTmpRecordName, sizeof(acTmpRecordName), 0, sizeof(acTmpRecordName));
    memset_s(acTmpStr, sizeof(acTmpStr), 0, sizeof(acTmpStr));

    CONDITION_TRUE_RTN_FAIL(!m_objXmlReader.get_string("/Result/RecordName", acTmpRecordName, 0));

    for (int i = 0; i < (int)strlen(acTmpRecordName); i++){
        if (acTmpRecordName[i] == cSepChr){
            strncpy_s(acTmpStr, MAX_TMP_BUFFER, (acTmpRecordName + 1) + iTmpPos , (unsigned int)(i - iTmpPos));
            acTmpStr[(unsigned int)(i - iTmpPos) - 1] = '\0';
            iTmpPos = i;

            iTmpSrcPos++;
            iTmpDstPos++;
        }
        else if ('\0' == acTmpRecordName[(unsigned int)(i + 1)]){
            strncpy_s(acTmpStr, MAX_TMP_BUFFER, (acTmpRecordName + 1) + iTmpPos , (unsigned int)((i - iTmpPos) + 1));
            iTmpSrcPos++;
            iTmpDstPos++;
        }
        else{
            continue;
        }

        if (m_strSrcName.size() != 0){
            if (!strcmp(acTmpStr, m_strSrcName.c_str())){
                m_iSrcPos = iTmpSrcPos;
            }
        }

        if (m_strDstName.size() != 0){
            if (!strcmp(acTmpStr, m_strDstName.c_str())){
                m_iDstPos = iTmpDstPos;
            }
        }
        else{
            return RETURN_ERR;
        }
    }

    CONDITION_TRUE_RTN_FAIL((-1 == m_iSrcPos) && (m_strSrcName.size() != 0));

    CONDITION_TRUE_RTN_FAIL(-1 == m_iDstPos);

    return RETURN_OK;
}

/*------------------------------------------------------------
Method       : int CRsltXmlParser::_ParseRecordValue()
Description  : Parse "/Result/RecordValue/Value" in tempary XML
               result file and storage in 'm_plstDstValue'.
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         :
Called by    :
Create By    :
Modification :
Others       :
------------------------------------------------------------*/
int CRsltXmlParser::_ParseRecordValue()
{
    char acTmpRecordValue[MAX_TMP_BUFFER] = {0};
    int iNum = 0;
    char cSepChr = ',';

    CONDITION_TRUE_RTN_FAIL(0 == (iNum = m_objXmlReader.get_count("/Result/RecordValue/Value")));

    for (int i = 0; i < iNum; i++){
        string strTmpSrcValue = "";
        string strTmpDstValue = "";
        int iTmpSrcPos = 0;
        int iTmpDstPos = 0;
        int iTmpPos = -1;

        char acTmpStr[MAX_TMP_BUFFER] = {0};
        wchar_t wt_buf[MAX_TMP_BUFFER * 2] = {0};

        CONDITION_TRUE_RTN_FAIL(!m_objXmlReader.get_string("/Result/RecordValue/Value", acTmpRecordValue, i));

        (void)SafeMultiByteToWideChar(wt_buf, acTmpRecordValue);
        (void)SafeWideCharToMultiByte(acTmpRecordValue, wt_buf);

        for (int j = 0; j < (int)strlen(acTmpRecordValue); j++){
            if (acTmpRecordValue[j] == cSepChr){
                strncpy_s(acTmpStr, MAX_TMP_BUFFER, (acTmpRecordValue + 1) + iTmpPos, (unsigned int)(j - iTmpPos));
                acTmpStr[(unsigned int)(j - iTmpPos) - 1] = '\0';
                iTmpPos = j;

                iTmpSrcPos++;
                iTmpDstPos++;
            }
            else if ('\0' == acTmpRecordValue[(unsigned int)(j + 1)]){
                strncpy_s(acTmpStr, MAX_TMP_BUFFER, (acTmpRecordValue + 1) + iTmpPos , (unsigned int)((j - iTmpPos) + 1));
                iTmpSrcPos++;
                iTmpDstPos++;
            }
            else{
                continue;
            }

            if (iTmpSrcPos == m_iSrcPos){
                strTmpSrcValue = string(acTmpStr);
            }

            if (iTmpDstPos == m_iDstPos){
                strTmpDstValue = string(acTmpStr);
            }
        }
        if (m_plstDstValue == NULL){
            return RETURN_ERR;
        }
                
        if (m_iSrcPos != -1){
            if (strTmpSrcValue == m_strSrcValue){
                m_plstDstValue->push_back(strTmpDstValue);
            }
        }
        else{
            m_plstDstValue->push_back(strTmpDstValue);
        }
    }

    return RETURN_OK;
}
