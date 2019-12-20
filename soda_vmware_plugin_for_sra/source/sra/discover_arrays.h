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

#ifndef DISCOVER_ARRAYS
#define DISCOVER_ARRAYS

#include "sra_basic.h"

int discover_arrays(XmlReader &reader);

class DiscoverArrays : public SraBasic
{
public:
    DiscoverArrays() : SraBasic() {};
    virtual ~DiscoverArrays() {};

protected:
    virtual void _write_response(XmlWriter &writer);
    virtual int _outband_process();

private:
    int _get_sysinfo(CCmdOperate& cmdOperate);
    int _get_peer_array(CCmdOperate& cmdOperate);

private:
    list<string> peer_arrays;
    list<string> peer_stretched;
    string array_name;
    string system_type;
};



#endif
