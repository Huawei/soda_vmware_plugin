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

#ifndef XMLREADER_H
#define XMLREADER_H

#include <string>
#include "xmlserializable.h"
#include "tinyxml.h"

class XmlReader
{
    public:
        XmlReader() : attrflag(0){xmldocument = NULL; parentelem = NULL;}
        virtual ~XmlReader();
        bool load_from_filename(const char *filename, TiXmlEncoding encoding );
        bool load_from_string(std::string xml, TiXmlEncoding encoding );
        bool get_string(const char *path,  char *value, int index ,int indexsecond = 0 ,int deep = 0);
        bool get_int(const char *path, int &value, int index);
        bool get_double(const char *path, double &value, int index);
        bool get_short(const char *path, short &value, int index);
        bool get_float(const char *path, float &value, int index);
        bool get_xml(const char* path, XmlSerializable* obj, int index);
        int  get_count(const char *path);
        int  get_count(const char *path,int index,int deep);

    private:
        XmlReader(const XmlReader&);
        XmlReader& operator=(const XmlReader&);

        bool set_document(TiXmlDocument *document);

        TiXmlElement* get_elem_bypath(const char *path, int index);
        TiXmlElement* get_elem_bypath(const char *path, int index,int indexsecond,int deep);
        TiXmlDocument *xmldocument;
        TiXmlElement  *parentelem;
        int attrflag;
};

#ifndef NODENUM
#define NODENUM  10
#endif

#ifndef NAMELEN
#define NAMELEN 256
#endif

#endif

