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

#include "xmlreader.h"
#include "xmlcommon.h"
#include "securec.h"

#ifdef WIN32
#pragma warning(disable: 4996)
#endif

XmlReader::~XmlReader()
{
    try{
        if (NULL != xmldocument){
            delete xmldocument;
            xmldocument = NULL;
        }
    }catch(...){}
}

bool XmlReader::set_document(TiXmlDocument * document)
{
    CHECK_NULL(document, false);

    xmldocument = document;

    return true;
}
//lint -e438
/*-------------------------------------------------------------------------
 Function Name: load_from_filename
 Description  : Load xml from file
? 1. Load xml from file
? 2. Initialize the current parent node element
 Data Accessed: None.
 Data Updated : None.
 Input        : filename
                encoding
 Output       : None.
 Return       : None.
 Call         : None.
 Called by    : None.
 Created By   :
 History        :
 1.Data         :
 Author       :
 Description  :
 Others         :
 -------------------------------------------------------------------------*/
bool XmlReader::load_from_filename(const char *filename, TiXmlEncoding encoding)
{
    bool ret = false;
 
    CHECK_NULL(filename, false);
    
    TiXmlDocument * document = new TiXmlDocument;
    CHECK_NULL(document, false);
    
    if(NULL != xmldocument){
        delete xmldocument;
        xmldocument = NULL;
    }

    ret = set_document(document);
    CHECK_FALSE(ret, false);
    document = NULL;

    ret = xmldocument->LoadFile(filename, encoding);
    CHECK_FALSE(ret, false);

    parentelem = xmldocument->RootElement();
    CHECK_NULL(parentelem, false);

    return ret;

}

/*-------------------------------------------------------------------------
 Function Name: load_from_string
 Description  : Load xml from string
 1. Load xml from a string
 2. Initialize the current parent node element
 Data Accessed: None.
 Data Updated : None.
 Input        : xml
                encoding
 Output       : None.
 Return       : None.
 Call         : None.
 Called by    : None.
 Created By   : 
 History        :
 1.Data         :
 Author       :
 Description  :
 Others         :
 -------------------------------------------------------------------------*/
bool XmlReader::load_from_string(std::string xml, TiXmlEncoding encoding)
{
    bool ret = false;
    TiXmlDocument * document = new TiXmlDocument;
    CHECK_NULL(document, false);

    if(NULL != xmldocument){
        delete xmldocument;
        xmldocument = NULL;

    }

    ret = set_document(document);
    CHECK_FALSE(ret, false);

    if (0 == xml.length()){
        return false;
    }

    xml += "\n"; 

    const char *temp = xmldocument->Parse(xml.c_str(), 0, encoding);
    CHECK_NULL(temp, false);

    parentelem = xmldocument->RootElement();
    CHECK_NULL(parentelem, false);

    return ret;

}

/*-------------------------------------------------------------------------
 Function Name: get_count
 Description  : Get the number of elements based on the path name
 Data Accessed: None.
 Data Updated : None.
 Input        : path:Node element path name
 Output       : None.
 Return       : None.
 Call         : None.
 Called by    : None.
 Created By   : 
 History        :
 1.Data         :
 Author       :
 Description  :
 Others         :
 -------------------------------------------------------------------------*/
int XmlReader::get_count(const char * path){
    int i = 0;
    int deepth = 0;
    int count  = 0;
    bool ret = false;
    char name_array[NODENUM][NAMELEN] = {0};

    bool flag = false;

    CHECK_NULL(path, 0);
    CHECK_NULL(parentelem, 0);

    TiXmlElement *tempelem = parentelem;

    ret = get_allelemname_bypath(path, name_array, flag);
    CHECK_FALSE(ret, 0);

    deepth = get_pathdeepth(path);
    CHECK_MORETHAN(1, deepth, 0);

    i = 1;
    do {
        tempelem = tempelem->FirstChildElement(name_array[i]);
        CHECK_NULL(tempelem, 0);
        i++;
    } while (i<deepth);

    for (; ; tempelem = tempelem->NextSiblingElement()){
        if(NULL == tempelem){
            break;
        }
        const char *value = tempelem->Value();

        if(value != NULL){
            if (!strncmp(name_array[deepth - 1], value, NAMELEN)){
                count++;
            }
        }

        if(NULL == (tempelem->NextSiblingElement())){
            break;
        }
    }

    return count;
}
int XmlReader::get_count(const char * path,int index,int deep)
{
    int i = 0;
    int j = 0;
    int deepth = 0;
    int count  = 0;
    bool ret = false;
    char name_array[NODENUM][NAMELEN] = {0};

    bool flag = false;

    CHECK_NULL(path, 0);
    CHECK_NULL(parentelem, 0);

    TiXmlElement *tempelem = parentelem;

    ret = get_allelemname_bypath(path, name_array, flag);
    CHECK_FALSE(ret, 0);

    deepth = get_pathdeepth(path);
    CHECK_MORETHAN(1, deepth, 0);

    i = 1;
    do {
        tempelem = tempelem->FirstChildElement(name_array[i]);
        CHECK_NULL(tempelem, 0);
        i++;
    } while (i<deepth-deep);

    while (j<index) {
        tempelem = tempelem->NextSiblingElement(name_array[i-1]);
        CHECK_NULL(tempelem, 0);
        j++;
    }
    for (j = 0; j < deep; j++){
        tempelem = tempelem->FirstChildElement(name_array[i]);
        CHECK_NULL(tempelem, 0);
        i++;
    }
    
    for (; ; tempelem = tempelem->NextSiblingElement()){
        if(NULL == tempelem){
            break;
        }
        const char *value = tempelem->Value();

        if(value != NULL){
            if (!strncmp(name_array[deepth - 1], value, NAMELEN)){
                count++;
            }
        }
        

        if(NULL == (tempelem->NextSiblingElement())){
            break;
        }
    }

    return count;
}
/*-------------------------------------------------------------------------
 Function Name: get_elem_bypath
 Description  : Get the element based on the path name, internal interface
 Data Accessed: None.
 Data Updated : None.
 Input        : Path: node element path name
 : index: node index, node index refers to the sibling number of the same name
 Output       : None.
 Return       : None.
 Call         : None.
 Called by    : None.
 Created By   :
 History        :
 1.Data         :
 Author       :
 Description  :
 Others         :
 -------------------------------------------------------------------------*/
TiXmlElement * XmlReader::get_elem_bypath(const char * path, int index)
{
    int i = 0;
    int deepth = 0;
    bool ret = false;
    bool end_flag = false;
    char name_array[NODENUM][NAMELEN] = {0};

    CHECK_NULL(path, NULL);
    CHECK_NULL(parentelem, NULL);
    CHECK_MORETHAN(0, index, NULL);

    TiXmlElement *tempelem = parentelem; 
    CHECK_NULL(tempelem, NULL);

    ret = get_allelemname_bypath(path, name_array, end_flag);
    CHECK_FALSE(ret, NULL);

    deepth = get_pathdeepth(path);
    CHECK_MORETHAN(1, deepth, NULL);

    if ('@' == name_array[deepth - 1][0]){
        attrflag = 1;
        deepth--;
    }

    for (i = 1; i < deepth; i++){
        tempelem = tempelem->FirstChildElement(name_array[i]);
        CHECK_NULL(tempelem, NULL);
    }


    i = 0;
    while (NULL != tempelem){
        const char *value = tempelem->Value();
        if(value != NULL){
            if (!strncmp(name_array[deepth - 1], value, NAMELEN)){
                i++;
            }
        }

        if ((index + 1) == i){
            break;
        }

        tempelem = tempelem->NextSiblingElement();
    }

    if ((index + 1) != i){
        return NULL;
    }

    return tempelem;
}

TiXmlElement * XmlReader::get_elem_bypath(const char * path, int index, int indexsecond,int deep)
{
    int i = 0, j = 0;
    int deepth = 0;
    bool ret = false;
    bool end_flag = false;
    char name_array[NODENUM][NAMELEN] = {0};

    CHECK_NULL(path, NULL);
    CHECK_NULL(parentelem, NULL);
    CHECK_MORETHAN(0, index, NULL);

    TiXmlElement *tempelem = parentelem;
    CHECK_NULL(tempelem, NULL);

    ret = get_allelemname_bypath(path, name_array, end_flag);
    CHECK_FALSE(ret, NULL);

    deepth = get_pathdeepth(path);
    CHECK_MORETHAN(1, deepth, NULL);

    if ('@' == name_array[deepth - 1][0]){
        attrflag = 1;
        deepth--;
    }

    for (i = 1; i < deepth-deep; i++){
        tempelem = tempelem->FirstChildElement(name_array[i]);
        CHECK_NULL(tempelem, NULL);
    }
    for (j = 0; j < indexsecond; j++){
        tempelem = tempelem->NextSiblingElement(name_array[i-1]);
        CHECK_NULL(tempelem, NULL);
    }
    for (j = 0; j < deep; j++){
        tempelem = tempelem->FirstChildElement(name_array[i]);
        CHECK_NULL(tempelem, 0);
        i++;
    }
    CHECK_NULL(tempelem, NULL);
    i = 0;
    while (NULL != tempelem){
        const char *value = tempelem->Value();
        if(value != NULL){
            if (!strncmp(name_array[deepth - 1], value, NAMELEN)){
                i++;
            }
        }

        if ((index + 1) == i){
            break;
        }

        tempelem = tempelem->NextSiblingElement();
    }

    if ((index + 1) != i){
        return NULL;
    }

    return tempelem;
}

bool XmlReader::get_string(const char *path, char *value, int index,int indexsecond,int deep)
{
    int deepth = 0;
    bool ret = false;
    bool end_flag = false;
    char temp[NAMELEN] = {0};
    char name_array[NODENUM][NAMELEN] = {0};
    TiXmlElement *tempelem = NULL;

    CHECK_NULL(path, false);
    CHECK_MORETHAN(0 , index, false);

    tempelem = get_elem_bypath(path, index,indexsecond,deep);
    CHECK_NULL(tempelem, false);

    if (1 == attrflag){
        ret = get_allelemname_bypath(path, name_array, end_flag);
        CHECK_FALSE(ret, false);

        deepth = get_pathdeepth(path);
        if (-1 == deepth){
            return false;
        }
        else{
            (void)strncpy_s(temp, sizeof(temp), name_array[deepth - 1] + 1, sizeof(temp)-1);
        }

        const char *attr = tempelem->Attribute(temp);
        CHECK_NULL(attr, false);

        memmove_s(value,strlen(attr)+1, attr,strlen(attr)+1);
        attrflag = 0;

        return true;
    }

    const char *text = tempelem->GetText();
    CHECK_NULL(text, true);

    memmove_s(value, strlen(text)+1, text, strlen(text)+1);
    return true;
}

bool XmlReader::get_int(const char * path, int &value, int index)
{
    char temp_value[NAMELEN] = {0};
    bool ret = false;

    CHECK_NULL(path, false);

    ret = get_string(path, temp_value, index);
    CHECK_FALSE(ret, false);

    if(sscanf_s(temp_value, "%d", &value) == -1){
        return false;
    }

    return ret;
}

bool XmlReader::get_double(const char * path, double &value, int index)
{
    char temp_value[NAMELEN] = {0};
    bool ret = false;

    CHECK_NULL(path, false);

    ret = get_string(path, temp_value, index); 
    CHECK_FALSE(ret, false);

    if(sscanf_s(temp_value, "%g", &value) == -1){
        return false;
    }
    //end
    return ret;
}

bool XmlReader::get_short(const char * path, short &value, int index)
{
    int temp_value = value;
    bool ret = false;

    ret = get_int(path, temp_value, index);
    CHECK_FALSE(ret, false);

    value = (short) temp_value;

    return ret;
}

bool XmlReader::get_float(const char * path, float & value, int index)
{
    double temp_value = value;
    bool ret = false;

    ret = get_double(path, temp_value, index);
    CHECK_FALSE(ret, false);

    value = (float)temp_value;

    return ret;
}

bool XmlReader::get_xml(const char * path, XmlSerializable * obj, int index)
{
    TiXmlElement *tempelem = parentelem;

    CHECK_NULL(path, false);
    CHECK_NULL(obj, false);
    CHECK_NULL(tempelem, false);

    parentelem = get_elem_bypath(path, index);
    if (NULL == parentelem){
        parentelem = tempelem;
        return false;
    }

    obj->readXml(this);
    parentelem = tempelem;

    return true;
}

