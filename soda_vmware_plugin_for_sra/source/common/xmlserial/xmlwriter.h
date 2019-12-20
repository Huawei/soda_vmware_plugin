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

#ifndef XMLWRITER_H
#define XMLWRITER_H
#include <string>
#include <iostream>
#include "tinyxml.h"
#include "xmlserializable.h"

using namespace std;

class XmlWriter
{
    public:
        XmlWriter(){flag = false;xmlreaderdoc = NULL;parentelem = NULL;string str = "Response";(void)xml_writer_init(str);};
        XmlWriter(const string& str);

        virtual ~XmlWriter();

        bool set_int(const char *path, int value, int index = 0);
        bool set_double(const char *path, double value, int index);
        bool set_float(const char *path, float value, int index);
        bool set_short(const char *path, short value, int index);
        bool set_string(const char *path, const char *value, int index = 0);
        bool set_xml(const char *path, XmlSerializable *obj);
        bool save_to_file(const char *filename);

    private:
        XmlWriter(const XmlWriter&);
        XmlWriter& operator=(const XmlWriter&);

        bool xml_writer_init(const string& str);
        TiXmlElement* get_last_elem_byname(int i, char name_array[][NAMELEN], int index, bool end_flag);

        TiXmlDocument *xmlreaderdoc;
        TiXmlElement *parentelem;
        bool flag; 
};

#ifndef NODENUM
#define NODENUM  10
#endif

#ifndef NAMELEN
#define NAMELEN  256
#endif

#endif

