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

#include "arrays.h"

/*-------------------------------------------------------------------------
Method       : ArraysInfo::ArraysInfo()
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
ArraysInfo::ArraysInfo()
{
    return;
}

/*-------------------------------------------------------------------------
Method       : ArraysInfo::~arraysinfo()
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
ArraysInfo::~ArraysInfo()
{
    return;
}

/*-------------------------------------------------------------------------
Method       : ArraysInfo::readXml()
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

void ArraysInfo::readXml(XmlReader *reader)
{
    (void)reader->get_xml((const char*) "/Response/Arrays", &array, 0);

    return;
}

/*-------------------------------------------------------------------------
Method       : ArraysInfo::writeXml()
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

void ArraysInfo::writeXml(XmlWriter *writer)
{
    (void)writer->set_xml((const char*)"/Response/Arrays", &array);

    return;
}


