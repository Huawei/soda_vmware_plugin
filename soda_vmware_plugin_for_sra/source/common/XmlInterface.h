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

#ifndef _XML_INTERFACE_H_
#define _XML_INTERFACE_H_


#define XML_COMMAND         "/Command"

#define XML_REQUSET_TITLE          "/Command/@xmlns"
#define XML_RESPONSE_TITLE         "/Response/@xmlns"
#define XML_OUTPUTFILE             "/Command/OutputFile"
#define XML_COMMANDNAME            "/Command/Name"
#define XML_STATUSFILE             "/Command/StatusFile"
#define XML_LOGLEVEL               "/Command/LogLevel"
#define XML_LOGDIRECTORY           "/Command/LogDirectory"
#define XML_USERNAME               "/Command/Connections/Connection/Username"
#define XML_PASSWD                 "/Command/Connections/Connection/Password"
#define XML_ARRAYSN                "/Command/Connections/Connection/Addresses/Address"
#define XML_ARRAYIP                "/Command/Connections/Connection/Addresses/Address"

#define XML_RESPONSE_ERROR          "/Response/Error/@code"

#define XML_PROGRESS               "/ProgressUpdate"
#define XML_PROGRESS_TITLE         "/ProgressUpdate/@xmlns"


#endif

