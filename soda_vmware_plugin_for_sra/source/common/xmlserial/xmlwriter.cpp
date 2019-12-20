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

#include "xmlwriter.h"
#include "xmlcommon.h"
#include "securec.h"

XmlWriter::XmlWriter(const string& str)
{
    flag = false;
    xmlreaderdoc = NULL;
    parentelem = NULL;
    (void)xml_writer_init(str);
};


XmlWriter::~XmlWriter()
{
    try{
        if (NULL != xmlreaderdoc){
            delete xmlreaderdoc;
            xmlreaderdoc = NULL;
        }
    }catch(...){}
}

/*-------------------------------------------------------------------------
Function Name: xml_writer_init
Description  : XmlWriter initialization, internal interface
1. Create an XML declaration header and initialize the root node
2. Initialize the current parent node element
Data Accessed: None.
Data Updated : None.
Input        : None.
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
bool XmlWriter::xml_writer_init(const string& str)
{
    xmlreaderdoc = new TiXmlDocument;
    CHECK_NULL(xmlreaderdoc, NULL);

    /* Define the xml file declaration header and insert it into the xml tree */
    TiXmlDeclaration *declaration = new TiXmlDeclaration(("1.0"),("UTF-8"),(""));
    if (NULL==declaration){
        delete xmlreaderdoc;
        xmlreaderdoc = NULL;
        return false;
    }

    (void)xmlreaderdoc->LinkEndChild(declaration);

    /* Create an xml root node and insert it into the xml tree */
    TiXmlElement *pRootEle = new TiXmlElement(str.c_str());
    if (NULL==pRootEle){
        delete xmlreaderdoc;
        xmlreaderdoc = NULL;
        return false;
    }

    if(NULL != xmlreaderdoc->LinkEndChild(pRootEle))
        parentelem = pRootEle;
    pRootEle = NULL;
    return true;
}

/*-------------------------------------------------------------------------
Function Name: get_last_elem_byname(int deepth, char name_array[][NAMELEN], int index, bool end_flag)
Description  : Get the node element according to the node element name, internal interface
Data Accessed: None.
Data Updated : None.
Input        : elemname
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
TiXmlElement* XmlWriter::get_last_elem_byname(int deepth, char name_array[][NAMELEN], int index, bool end_flag)
{

    TiXmlElement* elem = NULL;
    TiXmlElement* tempelem = NULL;

    if (!strncmp(parentelem->Value(),name_array[deepth], NAMELEN)){
        return parentelem;
    }

    elem = parentelem->FirstChildElement(name_array[deepth]);
    CHECK_NULL(elem, NULL);

    tempelem = elem;

    if (true == flag){
        for (tempelem = elem; tempelem != NULL; tempelem = tempelem->NextSiblingElement()){
            if (strncmp(name_array[deepth], tempelem->Value(), NAMELEN)){
                break;
            }
            else{
                elem = tempelem;
            }
        }

        return elem;
    }
    
    int i = 0;

    if((true == end_flag)&&(NULL != (elem->FirstAttribute()))){
        return NULL;
    }
    i = 1;
    for(; tempelem != NULL; tempelem = tempelem->NextSiblingElement()){
        if(i >= index){
            break;
        }

        if(!strncmp(name_array[deepth], tempelem->Value(), NAMELEN)){
            i++;
            elem = tempelem;
        }

    }

    if(i != (index + 1)){
        return NULL;
    }

    return elem;
}

/*-------------------------------------------------------------------------
Function Name: set_string
Description  : Set the value of the node element, the type of the value is a string
1. Insert node elements into the xml tree structure
2. Set the value of the node element
Data Accessed: None.
Data Updated : None.
Input        : Path: node element name
               value: the value of the node element
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
bool XmlWriter::set_string(const char* path, const char* value, int index)
{
    int deepth = 0;
    char temp[NAMELEN] = {0};
    bool end_flag = false;
    char name_array[NODENUM][NAMELEN] = {0};
    int i;

    TiXmlElement *tempelem;    
    TiXmlElement *tempparentelem2 = parentelem;

    CHECK_NULL(value, false);
    CHECK_NULL(path, false);

    if( !get_allelemname_bypath(path, name_array, end_flag)){
        return false;
    }

    deepth = get_pathdeepth(path);
    CHECK_MORETHAN(1, deepth, false);

    for (i = 0; i < deepth - 2; i++){
        tempelem = get_last_elem_byname(i, name_array, 0, false);

        std::string s = name_array[i];

        if ((NULL == tempelem) || flag){
            TiXmlElement *elem = new TiXmlElement(name_array[i]);
            CHECK_NULL(elem, false);

            if(NULL != parentelem->LinkEndChild(elem))
                parentelem = elem;            
        }
        else{
            parentelem = tempelem;
        }
    }

    if ('@' != name_array[deepth-1][0]){
        if(false == flag){
            flag = true;
            tempelem = get_last_elem_byname(i, name_array, 0, false);
            flag = false;
        }
        else{
            tempelem = get_last_elem_byname(i, name_array, 0, false);
        }

        if ((NULL == tempelem) || flag){
            TiXmlElement *elem = new TiXmlElement(name_array[i]);
            CHECK_NULL(elem, false);

            if(NULL != parentelem->LinkEndChild(elem))
                parentelem = elem;            
        }
        else{

            parentelem = tempelem;
        }

        tempelem = get_last_elem_byname(deepth-1, name_array, index, false);
        if ((NULL == tempelem) || flag){

            TiXmlElement *elem = new TiXmlElement(name_array[deepth-1]);
            CHECK_NULL(elem, false);
          
            if(NULL != parentelem->LinkEndChild(elem)){
                parentelem = elem;
            }
        }
        else{
            parentelem = tempelem;
        }

        flag = false; 

        const char* strValue = value;
        TiXmlText * elemValue = new TiXmlText(strValue);
        CHECK_NULL(elemValue, false);
        if(NULL != parentelem->LinkEndChild(elemValue))
            parentelem = tempparentelem2;
        return true;
    }
    else{
        if(true == end_flag){
            TiXmlElement *elem = new TiXmlElement(name_array[i]);
            CHECK_NULL(elem, false);
            
            if( NULL != parentelem->LinkEndChild(elem)){
                parentelem = elem;
                (void)strncpy_s(temp, sizeof(temp), name_array[deepth-1]+1, sizeof(temp)-1);
                parentelem->SetAttribute(temp, value);
            }
        }
        else{
            if (false == flag){
                flag = true;
                tempelem = get_last_elem_byname(i, name_array, 0, false);
                flag = false;
            }
            else{
                tempelem = get_last_elem_byname(i, name_array, 0, false);
            }

            if ((NULL == tempelem) || (flag)){
                TiXmlElement *elem = new TiXmlElement(name_array[i]);
                CHECK_NULL(elem, false);

                if(NULL != parentelem->LinkEndChild(elem))
                    parentelem = elem;
            }
            else{
                parentelem = tempelem;
            }

            memset_s(temp, sizeof(temp), 0, sizeof(temp));
            (void)strncpy_s(temp, sizeof(temp), name_array[deepth-1]+1, sizeof(temp)-1);
            parentelem->SetAttribute(temp, value);
        }
    }

    flag = false;
    parentelem = tempparentelem2;

    return true;
}

/*-------------------------------------------------------------------------
Function Name: set_int
Description  : Set the value of the node element, the type of the value is int
1. Insert node elements into the xml tree structure
2. Set the value of the node element
Data Accessed: None.
Data Updated : None.
Input        : Path: node element name
               value: the value of the node element
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
bool XmlWriter::set_int(const char *path, int value, int index)
{
    char temp_value[NAMELEN] = {0};

    CHECK_NULL(path, false);

#ifdef WIN32
    (void)_snprintf_s(temp_value, sizeof(temp_value), "%d", value);
#else
    sprintf_s(temp_value, sizeof(temp_value), "%d", value);
#endif

    return set_string(path, temp_value, index);
}

/*-------------------------------------------------------------------------
Function Name: set_double
Description  : Set the value of the node element, the type of the value is double
1. Insert node elements into the xml tree structure
2. Set the value of the node element
Data Accessed: None.
Data Updated : None.
Input        :Path: node element name
              value: the value of the node element
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
bool XmlWriter::set_double(const char *path, double value, int index)
{
    char temp_value[NAMELEN] = {0};

    CHECK_NULL(path, false);

#ifdef WIN32
    (void)_snprintf_s(temp_value, sizeof(temp_value), "%g", value);
#else
    (void)sprintf_s(temp_value, sizeof(temp_value), "%g", value);
#endif
    
    return set_string(path, temp_value, index);
}

/*-------------------------------------------------------------------------
Function Name: set_float
Description  : Set the value of the node element, the type of the value is float
1. Insert node elements into the xml tree structure
2. Set the value of the node element
Data Accessed: None.
Data Updated : None.
Input        : Path: node element name
               value: the value of the node element
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
bool XmlWriter::set_float(const char * path, float value, int index)
{
    double temp_value = value;

    CHECK_NULL(path, false);

    return set_double(path, temp_value, index);
}

/*-------------------------------------------------------------------------
Function Name: set_short
Description  : Set the value of the node element, the type of the value is short
1. Insert node elements into the xml tree structure
2. Set the value of the node element
Data Accessed: None.
Data Updated : None.
Input        : Path: node element name
              value: the value of the node element
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
bool XmlWriter::set_short(const char * path, short value, int index)
{
    int temp_value = value;

    CHECK_NULL(path, false);

    return set_int(path, temp_value, index);
}

/*-------------------------------------------------------------------------
Function Name: set_xml
Description  : Set the value of the node element, the type of the value is XmlSerializable
1. Insert node elements into the xml tree structure
2. Set the value of the node element
Data Accessed: None.
Data Updated : None.
Input        : path:
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
bool XmlWriter::set_xml(const char * path, XmlSerializable * obj)
{
    int deepth = 0;
    char name_array[NODENUM][NAMELEN] = {0};
    bool end_flag = false;
    TiXmlElement *tempelem = NULL;
    TiXmlElement *tempparentelem1 = parentelem;
    TiXmlElement *tempparentelem2 = parentelem;

    CHECK_NULL(obj, false);
    CHECK_NULL(path, false);
    CHECK_NULL(parentelem, false);

    flag = true; 
    if(!get_allelemname_bypath(path, name_array, end_flag)){
        return false;
    }

    deepth = get_pathdeepth(path);
    CHECK_MORETHAN(1, deepth, false);


    for (int i = 0; i < deepth; i++){
        tempelem = get_last_elem_byname(i, name_array, 0, false);

        if (NULL == tempelem){
            TiXmlElement *elem = new TiXmlElement(name_array[i]);
            CHECK_NULL(elem, false);

            if(NULL != tempparentelem1->LinkEndChild(elem))
                tempparentelem1 = elem;

            continue;
        }

        tempparentelem1 = tempelem;
    }

    parentelem = tempparentelem1;

    obj->writeXml(this); 

    parentelem = tempparentelem2;
    flag = false; 

    return true;
}

/*-------------------------------------------------------------------------
Function Name: save_to_file
Description  :
Data Accessed: None.
Data Updated : None.
Input        : filename
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
bool XmlWriter::save_to_file(const char * filename)
{
    bool ret = false;

    CHECK_NULL(filename, false);

    ret = xmlreaderdoc->SaveFile(filename);
    CHECK_FALSE(ret, false);

    return ret;
}