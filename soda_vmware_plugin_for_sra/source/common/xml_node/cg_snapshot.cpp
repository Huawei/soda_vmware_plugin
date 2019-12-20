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

#include "cg_snapshot.h"

/*-------------------------------------------------------------------------
Method       : CgSnapshot::writeXml()
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
void CgSnapshot::writeXml(XmlWriter *writer)
{
    if (!recover_info.rp_id.empty()){
        (void)writer->set_xml("/Snapshot", &recover_info);
    }

    if (!id.empty()){
        (void)writer->set_string("/Snapshot/@id", id.c_str());
    }

    if (!tg_devices_info.lst_target_devices.empty()){
        (void)writer->set_xml("/Snapshot", &tg_devices_info);
    }

    return;
}

/*-------------------------------------------------------------------------
Method       : CgSnapshot::readXml()
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
void CgSnapshot::readXml(XmlReader *reader)
{
    (void)reader;

    return;
}
