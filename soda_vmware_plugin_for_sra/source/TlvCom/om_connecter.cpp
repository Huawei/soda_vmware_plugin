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

#include "communication.h"

COMConnecter::COMConnecter(std::string _strDevice, unsigned short _usPort)
: m_strDevice(_strDevice), m_usPort(_usPort)
{
    m_connUp = false;
    m_FlagOut = true;
}

int COMConnecter::init()
{
#ifndef WIN32
    
    
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    ACE_OS::sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    ACE_OS::sigaction(SIGPIPE, &act, 0);
#endif

    if (!m_connUp)
    {
        ACE_Guard<ACE_Thread_Mutex> oGuard(m_oMutex);
        if (!m_connUp)
        {
            return setupConnection_i();
        }
    }

    return RETURN_OK;
}

int COMConnecter::unInit()
{
    if (m_connUp)
    {
        ACE_Guard<ACE_Thread_Mutex> oGuard(m_oMutex);
        if (m_connUp)
        {
            return closeConnection_i();
        }
    }

    return RETURN_OK;
}

int COMConnecter::sendMsg(CS5KPackageV3 &msg, const int iTimeOut)
{
    unsigned char * pBuffer = NULL;
    int iBufferLen = 0;
    iBufferLen = msg.encode(pBuffer);
    unsigned short msn = msg.head().usSerial;
    ACE_Guard<ACE_Thread_Mutex> oGuard(m_oMutex);

    int iRet = 0;
    if(!m_connUp)
    {
        iRet = setupConnection_i();
        if(RETURN_OK != iRet)
        {
            return iRet;
        }
    }

    ACE_Time_Value timeout(SEND_WAIT_TIME);
    
    if (m_FlagOut)
    {
        iRet = m_connToOMOUT.send_n(pBuffer, (size_t)iBufferLen, 0, &timeout);
    }
    else
    {
        iRet = m_connToOM.send(pBuffer, (size_t)iBufferLen, &timeout);
    }
    
    if (iRet <= 0)
    {
        int ret = SOCKET_SEND_MSG_FAILED;
        ostringstream oss;
        msg.dump(oss);
        if(iRet < 0 && ACE_OS::last_error() == ETIME)
        {
            ret = SOCKET_SEND_MSG_TIME_OUT;
        }

        delete [] pBuffer;
        
        reconnect_i();

        return ret;
    }

    delete [] pBuffer;
    ostringstream oss;
    oss<<"send msg:";
    msg.dump(oss);
    do
    {
        iRet = receiveMsg(msg, iTimeOut);

    }while(iRet == RETURN_OK && msg.head().usSerial != msn );
    return iRet;
}

int COMConnecter::receiveMsgHead(CS5KPackageV3 & msg,unsigned char *buf,int iTimeOut)
{
    ACE_Time_Value timeout(iTimeOut);
    int iNumBytes = 0;
    if (m_FlagOut)
    {
        iNumBytes = m_connToOMOUT.recv_n(buf, sizeof(S5KMsgHeadV3), 0, &timeout);
    }
    else
    {
        iNumBytes = m_connToOM.recv_n(buf, sizeof(S5KMsgHeadV3), 0, &timeout);
    }
    if (iNumBytes <= 0)
    {
        ostringstream oss;
            msg.dump(oss);
        int ret = SOCKET_RECEIVE_MSG_FAILED;
        if(iNumBytes<0 && ACE_OS::last_error()== ETIME)
        {
            ret = SOCKET_RECEIVE_MSG_TIME_OUT;
        }
        return ret;
    }
    return RETURN_OK;
}
int COMConnecter::receiveMsgBody(CS5KPackageV3 & msg,unsigned char *buf,int iRemainLen,int iTimeOut)
{
    int iNumBytes = 0;
    ACE_Time_Value timeout(iTimeOut);
    
    if(0 < iRemainLen)
    {
        if (m_FlagOut)
        {
            iNumBytes = m_connToOMOUT.recv_n(buf, iRemainLen, 0, &timeout);
        }
        else
        {
            iNumBytes = m_connToOM.recv_n(buf, iRemainLen, 0, &timeout);
        }
        if (iNumBytes <= 0)
        {
            ostringstream oss;
            msg.dump(oss);

            return SOCKET_RECEIVE_MSG_FAILED;
        }
    }
    return RETURN_OK;
}

int COMConnecter::receiveMsg(CS5KPackageV3 & msg, const int iTimeOut)
{
    int iTotal = 0;

    ACE_Auto_Array_Ptr<unsigned char> pcRead(new unsigned char[LSOCK_BUF_SIZE]);

    int headlen=sizeof(S5KMsgHeadV3);
    int iRemainLen = 0;
    
    int ret = receiveMsgHead(msg,pcRead.get(),  iTimeOut);
    if (ret != RETURN_OK)
    {
        
        reconnect_i();
        return ret;
    }

    iTotal += headlen;
    S5KMsgHeadV3* pMsgHead = (S5KMsgHeadV3*)pcRead.get();
    iRemainLen = (int)pMsgHead->uiLength;
    if(iRemainLen+sizeof(S5KMsgHeadV3)>LSOCK_BUF_SIZE)
    {
        return SOCKET_RECEIVE_MSG_FAILED;
    }

    ret = receiveMsgBody(msg,pcRead.get() + headlen, iRemainLen, iTimeOut);
    if (ret != RETURN_OK)
    {
        reconnect_i();
        return ret;
    }

    iTotal += iRemainLen;


    CS5KPackageV3 rsp;
    int iRet = rsp.decode(pcRead.get(), iTotal);
    if(RETURN_ERROR == iRet)
    {
        return SOCKET_DECODE_MESSAGE_FAILED;
    }
    ostringstream oss;
    oss<<"receive msg:";
    rsp.dump(oss);

    
    if (msg.head().usSerial != pMsgHead->usSerial)
    {

        oss.str("");
        oss<<"receive msg sn not match request msg sn. req:";
        msg.dump(oss);
        oss<<"rsp:";
        rsp.dump(oss);
    }
    msg = rsp;
    return RETURN_OK;
}

string COMConnecter::getPeerAddr()
{
    std::stringstream oss;
    oss<< m_strDevice.c_str() << ":" << m_usPort;
    return oss.str();
}

int COMConnecter::reconnect()
{
    ACE_Guard<ACE_Thread_Mutex> oGuard(m_oMutex);
    return reconnect_i();
}

int COMConnecter::reconnect_i()
{
    closeConnection_i();
    return setupConnection_i();
}
int COMConnecter::closeConnection_i()
{
    
    int ret = 0;
    if (m_FlagOut)
    {
        ret = m_connToOMOUT.close();
    }
    else
    {
        ret = m_connToOM.close();
    }
    
    if (0 > ret)
    {
        return RETURN_ERROR;
    }
    

    m_connUp = false;

    return RETURN_OK;
}

int COMConnecter::setupConnection_i()
{
    int ret = 0;
    ACE_INET_Addr omAddr;
    omAddr.string_to_addr(getPeerAddr().c_str());

    ACE_Time_Value timeout(CONNECT_WAIT_TIME);
    
    ACE_SOCK_Connector connector;
    
    ACE_SSL_SOCK_Connector connectorOUT;
    if (m_FlagOut)
    {
        ret = connectorOUT.connect(m_connToOMOUT, omAddr, &timeout);
    }
    else
    {
        ret = connector.connect(m_connToOM, omAddr, &timeout);
    }

    if (0 > ret)
    {
        return SOCKET_CONNECT_FAILED;
    }

    m_connUp = true;

    return 0;
}

void COMConnecter::SetFlagOUT(bool bFlag)
{
    m_FlagOut = bFlag;
}




