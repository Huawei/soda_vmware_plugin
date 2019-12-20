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

#include "commond_info.h"

void ErrorInfo::readXml(XmlReader *reader)
{
    (void)reader;
}

void ErrorInfo::writeXml(XmlWriter *writer)
{
    if (desc.empty()){
        (void)writer->set_string("/Error/@code/", code.c_str(), 0);

        return;
    }
    if (!code.empty()){
        (void)writer->set_string("/Error/@code", code.c_str(), 0);
    }

    (void)writer->set_string("/Error/Description", desc.c_str(), 0);

    if (hint.empty() && !desc.empty()){
        hint = "Please check the information from ISM";
    }

    if (!hint.empty()){
        (void)writer->set_string("/Error/FixHint", hint.c_str(), 0);
    }

    return;
}

bool ErrorInfo::hasError()
{
    if (code.empty() && desc.empty()){
        return false;
    }

    return true;
}

void WarnInfo::readXml(XmlReader *reader)
{
    (void)reader;
}

void WarnInfo::writeXml(XmlWriter *writer)
{
    if (desc.empty()){
        (void)writer->set_string("/Warning/@code/", code.c_str(), 0);

        return;
    }
    if (!code.empty()){
        (void)writer->set_string("/Warning/@code", code.c_str(), 0);
    }

    (void)writer->set_string("/Warning/Description", desc.c_str(), 0);

    if (hint.empty() && !desc.empty()){
        hint = "Please check the information from ISM";
    }

    if (!hint.empty()){
        (void)writer->set_string("/Warning/FixHint", hint.c_str(), 0);
    }

    return;
}

bool WarnInfo::hasWarning()
{
    if (code.empty()){
        return false;
    }

    return true;
}

void Warnings::readXml(XmlReader *reader)
{
    (void)reader;
}

void Warnings::writeXml(XmlWriter *writer)
{
    list<WarnInfo>::iterator iter = lst_warn.begin();

    while (iter != lst_warn.end()){
        (void)writer->set_xml("/Warnings", &(*iter));

        ++iter;
    }

    return;
}

void ErrorDefinition::readXml(XmlReader *reader)
{
    (void)reader;
}

void ErrorDefinition::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/ErrorDefinition/@code", code.c_str(), 0);
    
    Description dp;
    dp.content = desc;
    dp.str_id = desc_id;
    (void)writer->set_xml("/ErrorDefinition", &dp);

    FixHint fh;
    fh.content = hint;
    fh.str_id = hint_id;
    (void)writer->set_xml("/ErrorDefinition", &fh);

    return;
}

void WarningDefinition::readXml(XmlReader *reader)
{
    (void)reader;
}

void WarningDefinition::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/WarningDefinition/@code", code.c_str(), 0);

    Description dp;
    dp.content = desc;
    dp.str_id = desc_id;
    (void)writer->set_xml("/WarningDefinition", &dp);
    
    FixHint fh;
    fh.content = hint;
    fh.str_id = hint_id;
    (void)writer->set_xml("/WarningDefinition", &fh);

    return;
}

void ErrorDefinitions::readXml(XmlReader *reader)
{
    (void)reader;
}

void ErrorDefinitions::writeXml(XmlWriter *writer)
{
    list<ErrorDefinition>::iterator iter = lst_err.begin();

    while (iter != lst_err.end()){
        (void)writer->set_xml("/ErrorDefinitions", &(*iter));

        ++iter;
    }
}

void WarningDefinitions::readXml(XmlReader *reader)
{
    (void)reader;
}

void WarningDefinitions::writeXml(XmlWriter *writer)
{
    list<WarningDefinition>::iterator iter = lst_warn.begin();

    while (iter != lst_warn.end()){
        (void)writer->set_xml("/WarningDefinitions", &(*iter));

        ++iter;
    }
}

void FixHint::readXml(XmlReader *reader)
{
    (void)reader;
}

void FixHint::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/FixHint/@stringId", str_id.c_str());
    (void)writer->set_string("/FixHint", content.c_str());

    return;
}

void Description::readXml(XmlReader *reader)
{
    (void)reader;
}

void Description::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/Description/@stringId", str_id.c_str());
    (void)writer->set_string("/Description", content.c_str());

    return;
}
