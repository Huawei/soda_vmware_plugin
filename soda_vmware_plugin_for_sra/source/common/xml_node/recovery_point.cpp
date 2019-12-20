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

#include "recovery_point.h"

RecoveryPointInfo::RecoveryPointInfo()
{
    return;
}

RecoveryPointInfo::~RecoveryPointInfo()
{
    return;
}

void RecoveryPointInfo::readXml(XmlReader *reader)
{
    (void)reader->get_string((const char*) "/RecoveryPoint/@id", (char *)rp_id.c_str(), 0);
    (void)reader->get_string((const char*) "/RecoveryPoint/Name", (char *)rp_name.c_str(), 0);
    (void)reader->get_string((const char*) "/RecoveryPoint/Time", (char *)rp_time.c_str(), 0);

    return;
}

void RecoveryPointInfo::writeXml(XmlWriter *writer)
{
    if (!rp_id.empty()){
        (void)writer->set_string("/RecoveryPoint/@id", rp_id.c_str(), 0);
    }

    if (!rp_time.empty()){
        (void)writer->set_string("/RecoveryPoint/Time", rp_time.c_str(), 0);
    }

    return;
}


