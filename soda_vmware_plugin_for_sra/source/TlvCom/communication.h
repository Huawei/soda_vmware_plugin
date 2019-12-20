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

#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#pragma warning(disable:4996)
#pragma warning(disable:4267)

#include <iostream>
#include <sstream>
#include <string>

#include <ace/SSL/SSL_SOCK_Connector.h>
#include <ace/SOCK_Connector.h>
#include <ace/Auto_Ptr.h>
#include <ace/OS.h>
#include <ace/SOCK_Stream.h>
#include <ace/Task.h>

#include "msg_package.h"
#include "communcation_export.h"

using namespace std;

#define  SPECIAL_SESSION_KEY  4283979604UL
#define  SEND_TIME_OUT  600

#define  RETURN_OK 0
#define  RETURN_WAR 5
#define  RETURN_ERROR -1
#define  SOCKET_CONNECT_FAILED 256
#define  SOCKET_SEND_MSG_FAILED 257
#define  SOCKET_RECEIVE_MSG_FAILED 258
#define  SOCKET_SERIAL_DONT_MATCH 259
#define  SOCKET_DECODE_MESSAGE_FAILED 260
#define  SOCKET_SEND_MSG_TIME_OUT 263
#define  SOCKET_RECEIVE_MSG_TIME_OUT 264

class CConnecter
{
public:
    virtual int init() = 0;

    virtual int unInit() = 0;

    virtual int sendMsg(CS5KPackageV3 & msg, const int iTimeOut = SEND_TIME_OUT) = 0;
    virtual int reconnect()=0;

    virtual ~CConnecter(){}
};

class COMMUNICATION_API COMConnecter : public CConnecter
{
    public:
    enum
    {
        CONNECT_WAIT_TIME=2,
        RECV_WAIT_TIME=310,    
        SEND_WAIT_TIME=30,  
        LSOCK_BUF_SIZE=10*1024*1024 
    };

    COMConnecter(std::string _strDevice, unsigned short _usPort);

    virtual int init();

    virtual int unInit();

    virtual int sendMsg(CS5KPackageV3 & msg, const int iTimeOut = SEND_TIME_OUT);
    virtual int reconnect();

    void SetFlagOUT(bool bFlag);

    protected:
    int closeConnection_i();

    int setupConnection_i();

    int reconnect_i();

    int receiveMsg(CS5KPackageV3 & msg, const int iTimeOut);

    std::string getPeerAddr();

    int receiveMsgHead(CS5KPackageV3 & msg,unsigned char *buf,int iTimeOut);

    int receiveMsgBody(CS5KPackageV3 & msg,unsigned char *buf,int iRemainLen,int iTimeOut);
    private:

    std::string m_strDevice;
    unsigned short m_usPort;

    bool m_connUp;
    ACE_SOCK_Stream m_connToOM;
    bool m_FlagOut;
    ACE_SSL_SOCK_Stream m_connToOMOUT; 
    ACE_Thread_Mutex m_oMutex; 
};

typedef struct tag_OUT_ARRARYLINK_INFO
{
    list<string> lstArrayIP;
    string strArrayUser;
    string strArrayPwd;
    string strUserID;
    unsigned int uiSessionKey;
    COMConnecter* pobjOMConnector; 
}OUT_ARRARYLINK_INFO_STRU;

#endif //_COMMUNICATION_H_

