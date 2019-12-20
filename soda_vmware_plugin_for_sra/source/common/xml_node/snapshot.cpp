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

#include "snapshot.h"

SnapShotInfo::SnapShotInfo()
{
    return;
}

SnapShotInfo::~SnapShotInfo()
{
    return;
}

void SnapShotInfo::readXml(XmlReader *reader)
{
    (void)reader;
    return;
}

void SnapShotInfo::writeXml(XmlWriter *writer)
{
    if (!snap_id.empty()){
        (void)writer->set_string("/Snapshot/@id", snap_id.c_str(), 0);
    }

    if (!snap_wwn.empty()){
        (void)writer->set_string("/Snapshot/Identity/Wwn", snap_wwn.c_str(), 0);
    }

    if (!snap_fspath.empty()){
        (void)writer->set_string("/Snapshot/Identity/NfsName", snap_fspath.c_str(), 0);
    }

    if (!recoverypoint_info.rp_id.empty()){
        (void)writer->set_xml("/Snapshot", &recoverypoint_info);
    }

    return;
}



