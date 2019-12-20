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

#include "array.h"

/*-------------------------------------------------------------------------
Method       : ArrayInfo::ArrayInfo()
Description  : 
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         : None.
Called by    : None.
Created By   : 
History      :
1.Date       :
  Author     :
  Description:
Others       :
-------------------------------------------------------------------------*/
ArrayInfo::ArrayInfo()
{
    memset_s(id, sizeof(id), 0, sizeof(id));
    memset_s(name, sizeof(name), 0, sizeof(name));

    return;
}

/*-------------------------------------------------------------------------
Method       : ArrayInfo::~ArrayInfo()
Description  : 
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         : None.
Called by    : None.
Created By   :
History      :
1.Date       :
  Author     :
  Description:
Others       :
-------------------------------------------------------------------------*/
ArrayInfo::~ArrayInfo()
{
    return;
}

/*-------------------------------------------------------------------------
Method       : ArrayInfo::readXml()
Description  : read XML file
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         : None.
Called by    : None.
Created By   : 
History      :
1.Date       :
  Author     :
  Description:
Others       :
-------------------------------------------------------------------------*/
void ArrayInfo::readXml(XmlReader *reader)
{
    list<string>::iterator it = peer_arrays.begin();

    if(!reader->get_string((const char*) "/Array/@id", id, 0)){
        return;
    }
    if(!reader->get_string((const char*) "/Array/Name", name, 0)){
        return;
    }

    while (it != peer_arrays.end()){
        char str[LENGTH_COMMON] = {0};

        if(reader->get_string("/Array/PeerArrays/PeerArray/@id", str, 0)){
            peer_arrays.push_back(str);
        }
        ++it;
    }

    return;
}

/*-------------------------------------------------------------------------
Method       : ArrayInfo::writeXml()
Description  : write XML file
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         : None.
Called by    : None.
Created By   : 
History      :
1.Date       :
  Author     :
  Description:
Others       :
-------------------------------------------------------------------------*/
void ArrayInfo::writeXml(XmlWriter *writer)
{
    list<string>::iterator it = peer_arrays.begin();
    list<string>::iterator its = peer_stretched.begin();

    PeerArrays arrays;

    const string id_err("Write /Array id error.");
    const string name_err("Write /Array/Name error.");
    const string model_err("Write /Array/Model/Name error.");

    if (!writer->set_string("/Array/Name", name)){
        throw XmlException(ERROR_INTERNAL_PROCESS_FAIL, name_err);
    }

    if (!writer->set_string("/Array/@id", id)){
        throw XmlException(ERROR_INTERNAL_PROCESS_FAIL, id_err);
    }

    (void)writer->set_xml("/Array", &model);
    (void)writer->set_xml("/Array", &software);

    while (it != peer_arrays.end()){
        PeerArray peer;
        peer.id = *it;
        if (g_bstretch){
            peer.strtched = *its;
        }

        arrays.lst_peers.push_back(peer);

        ++it;
    }

    (void)writer->set_xml("/Array", &arrays);

    return;
}

/*-------------------------------------------------------------------------
Method       : ModelInfo::writeXml()
Description  : write XML file
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         : None.
Called by    : None.
Created By   : 
History      :
1.Date       :
  Author     :
  Description:
Others       :
-------------------------------------------------------------------------*/
void ModelInfo::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/Model/Name", model_name.c_str());
    (void)writer->set_string("/Model/Vendor/@stringId", "OceanStorVendor");
    (void)writer->set_string("/Model/Vendor", vender_name.c_str());

    return;
}

/*-------------------------------------------------------------------------
Method       : ModelInfo::readXml()
Description  : read XML file
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         : None.
Called by    : None.
Created By   :
History      :
1.Date       :
  Author     :
  Description:
Others       :
-------------------------------------------------------------------------*/
void ModelInfo::readXml(XmlReader *reader)
{
    (void)reader;

    return;
}

/*-------------------------------------------------------------------------
Method       : ReplicationSoftware::writeXml()
Description  : write XML file
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         : None.
Called by    : None.
Created By   : 
History      :
1.Date       :
  Author     :
  Description:
Others       :
-------------------------------------------------------------------------*/
void ReplicationSoftware::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/ReplicationSoftware/Name", software_name.c_str());
    (void)writer->set_string("/ReplicationSoftware/Version", software_version.c_str());

    return;
}

/*-------------------------------------------------------------------------
Method       : ReplicationSoftware::readXml()
Description  : read XML file
Data Accessed: None.
Data Updated : None.
Input        : None.
Output       : None.
Return       : None.
Call         : None.
Called by    : None.
Created By   : 
History      :
1.Date       :
  Author     :
  Description:
Others       :
-------------------------------------------------------------------------*/
void ReplicationSoftware::readXml(XmlReader *reader)
{
    (void)reader;

    return;
}

void PeerArray::writeXml(XmlWriter *writer)
{
    (void)writer->set_string("/PeerArray/@id", id.c_str());
    if (g_bstretch){
        (void)writer->set_string("/PeerArray/@stretchedStorage", strtched.c_str());
    }
}

void PeerArray::readXml(XmlReader *reader)
{
    (void)reader;
}

void PeerArrays::writeXml(XmlWriter *writer)
{
    list<PeerArray>::iterator iter_peers;

    for (iter_peers = lst_peers.begin(); iter_peers != lst_peers.end(); ++iter_peers){
        (void)writer->set_xml("/PeerArrays", &(*iter_peers));
    }
}

void PeerArrays::readXml(XmlReader *reader)
{
    (void)reader;
}

