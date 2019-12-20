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

#ifndef _MSG_PACKAGE_H_
#define _MSG_PACKAGE_H_

#pragma warning(disable:4996)
#pragma warning(disable:4267)
#pragma warning(disable:4018)
#pragma warning(disable:4098)
#pragma warning(disable:4075)
#pragma warning(disable:4244)

#include <list>
#include <limits>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <ace/OS.h>
#include <ace/Hash_Map_Manager_T.h>
#include <ace/Null_Mutex.h>
#include <ace/Thread_Mutex.h>
#include <ace/Containers.h>
#include <ace/Atomic_Op_T.h>
#include <vector>
#include <json/json.h>

#include "communcation_export.h"
#include "cli_exception.h"
#include "securec.h"

#ifdef WIN32
#pragma warning(disable: 4200)
#endif


const unsigned int S5K_MSG_V1 = 1;
const unsigned int S5K_MSG_V2 = 2;
const unsigned int S5K_MSG_V3 = 3;


const unsigned int MASTER_NODE = 0xFFFD;
const unsigned int LOCAL_NODE = 0xFFFF;
const unsigned int OPPOSITE_NODE = 0xFFFE;


const unsigned int MAGIC_NUM = 0x55555555;



#pragma pack(4) 
struct S5KMsgHead
{
    unsigned int uiResult;
    unsigned int uiCmdCode;
    unsigned int uiLength;
    
    union MsgParam
    {
        unsigned int ulPara;
        char cParam[4];
    }Param[0];  
};

struct S5KMsgHeadV2
{
    unsigned int uiMagic;
    unsigned int uiVersion;
    unsigned int uiResult;
    unsigned int uiSkey;    
    unsigned long long ullCmdCode;
    unsigned int uiLength;
    
    union MsgParam
    {
        unsigned int ulPara;
        char cParam[4];
    }Param[0];  
};


struct Enum_T
{
    Enum_T(){value=0;}
    Enum_T(unsigned long long v):value(v){}
    unsigned long long value;
};


struct Time_T
{
    Time_T(){value=0;}
    Time_T(unsigned long long v):value(v){}
    unsigned long long value;
};

class COMMUNICATION_API S5KMsgHeadV3
{
    friend class CS5KPackageV3;

public:
    unsigned int uiMagicNumber;
    unsigned int uiVersion;
    unsigned int uiStatus;
    unsigned int uiSessionKey;
    unsigned short usNodeID;
    unsigned short usReserved;
    unsigned short usBoardID;
    unsigned short usSerial;
    unsigned int uiLength;

public:
    S5KMsgHeadV3()
    {
        uiMagicNumber = MAGIC_NUM;
        uiVersion = S5K_MSG_V3;
        uiStatus = 0;
        uiSessionKey = 0xffffffff;
        
        usNodeID = MASTER_NODE;
        usReserved = 0xffff;
        usBoardID = 0xffff;
        usSerial = 0xffff;
        uiLength = 0;
    }

    S5KMsgHeadV3(const S5KMsgHeadV3& other)
    {
        this->uiStatus = other.uiStatus;
        this->uiSessionKey = other.uiSessionKey;
        this->usNodeID = other.usNodeID;
        this->usBoardID = other.usBoardID;
        this->usSerial = other.usSerial;
        this->uiLength = other.uiLength;
        this->usReserved = other.usReserved;
    }

    S5KMsgHeadV3& operator=(const S5KMsgHeadV3& other)
    {
        this->uiStatus = other.uiStatus;
        this->uiSessionKey = other.uiSessionKey;
        this->usNodeID = other.usNodeID;
        this->usBoardID = other.usBoardID;
        this->usSerial = other.usSerial;
        this->uiLength = other.uiLength;
        this->usReserved = other.usReserved;
        return *this;
    }
};

#pragma pack()  


class  COMMUNICATION_API CS5kPkgBase
{
public:
    CS5kPkgBase() {}
    virtual ~CS5kPkgBase() {}
    virtual int decode(unsigned char* pBuf, unsigned int uiBufSize) = 0;
    virtual int encode(unsigned char*& pBuf) = 0;
    virtual int encode(unsigned char* pBuf,int bufsize) = 0;
    virtual void SessionKey(unsigned int uiSessionKey) = 0;
    virtual unsigned int SessionKey(void) const = 0;
    virtual void result(unsigned int iResult) = 0;
    virtual unsigned int result(void) const = 0;
    virtual unsigned int version(void) const = 0;
    virtual void clear(void) = 0;
    virtual void dump(std::ostream& str) const = 0;
};


#ifdef WIN32

#pragma warning(disable: 4290)

#pragma  warning(disable: 4251)
#endif


typedef unsigned short tag_t;

COMMUNICATION_API extern tag_t TLV_RETURN_CODE;


const int MIN_TAG_VALUE = 0;

const int MAX_TAG_VALUE = 0xFFF;
const int DEFAULT_TAG_VALUE = 0xFFE;

template <typename T>
size_t GetFixSize(T value)
{
    return sizeof(value);
}



class COMMUNICATION_API CTLVCell
{
    friend class CS5KPackageV3;
public:
    enum
    {
        e_TypeBitSize = 4,      
        e_TagBitSize = 12,      
        e_TypeBitMask = 0x000F, 
        e_TagBitMask = 0xFFF0   
    };

public:
    enum ETLVType
    {
        e_Invalid = 0,          
        e_Octet,                
        e_String,               
        e_Uint,                 
        e_Int,                   
        e_Bool,
        e_Enum,
        e_Array,
        e_Json,
        e_Time,
        ETLVType_END
    };

    CTLVCell(tag_t tag = DEFAULT_TAG_VALUE) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set_i(e_Invalid, tag, NULL, usLength_);
    }

    CTLVCell(tag_t tag, char cValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, cValue);
    }

    CTLVCell(tag_t tag, unsigned char ucValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, ucValue);
    }

    CTLVCell(tag_t tag, short sValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, sValue);
    }

    CTLVCell(tag_t tag, unsigned short usValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, usValue);
    }

    CTLVCell(tag_t tag, int iValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, iValue);
    }

    CTLVCell(tag_t tag, unsigned int uiValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, uiValue);
    }

    CTLVCell(tag_t tag, long lValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, lValue);
    }

    CTLVCell(tag_t tag, unsigned long ulValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, ulValue);
    }

    CTLVCell(tag_t tag, long long llValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, llValue);
    }

    CTLVCell(tag_t tag, unsigned long long ullValue) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, ullValue);
    }

    CTLVCell(tag_t tag, const char* pString) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, pString);
    }
    
    CTLVCell(tag_t tag, const std::string& str) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, str.c_str());
    }

    CTLVCell(tag_t tag, const char* pValue, size_t length) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, pValue, length);
    }

    CTLVCell(tag_t tag, const Json::Value &v) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, v);
    }

    CTLVCell(tag_t tag, bool v) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        
        
        set_bool(tag, v);
    }

    CTLVCell(tag_t tag, Enum_T v) : usMixTag_(0), usLength_(0), pValue_(NULL)
    {
        set(tag, v);
    }

    ~CTLVCell()
    {
        delete [] pValue_;
        pValue_ = NULL;
    }

    CTLVCell(const CTLVCell& other);

    CTLVCell& operator =(const CTLVCell& other);

    CTLVCell& operator=(char cValue)
    {
        this->set(this->tag(), cValue);
        return *this;
    }

    CTLVCell& operator=(unsigned char ucValue)
    {
        this->set(this->tag(), ucValue);
        return *this;
    }

    CTLVCell& operator=(short sValue)
    {
        this->set(this->tag(), sValue);
        return *this;
    }

    CTLVCell& operator=(unsigned short usValue)
    {
        this->set(this->tag(), usValue);
        return *this;
    }

    CTLVCell& operator=(int iValue)
    {
        this->set(this->tag(), iValue);
        return *this;
    }

    CTLVCell& operator=(unsigned int uiValue)
    {
        this->set(this->tag(), uiValue);
        return *this;
    }

    CTLVCell& operator=(long long llValue)
    {
        this->set(this->tag(), llValue);
        return *this;
    }

    CTLVCell& operator=(unsigned long long ullValue)
    {
        this->set(this->tag(), ullValue);
        return *this;
    }

    CTLVCell& operator=(const char * pValue)
    {
        this->set(this->tag(), pValue);
        return *this;
    }

    CTLVCell& operator=(const Json::Value &v)
    {
        this->set(this->tag(), v);
        return *this;
    }

    CTLVCell& operator=(bool v)
    {
        this->set_bool(this->tag(), v);
        return *this;
    }

    CTLVCell& operator=(Enum_T v)
    {
        this->set(this->tag(), v);
        return *this;
    }
    CTLVCell& operator=(Time_T v)
    {
        this->set(this->tag(), v);
        return *this;
    }

    void set(tag_t tag, char cValue)
    {
        set_i(e_Int, tag, &cValue, GetFixSize(cValue));
    }

    void set(tag_t tag, unsigned char ucValue)
    {
        set_i(e_Uint, tag, &ucValue, GetFixSize(ucValue));
    }

    void set(tag_t tag, short sValue)
    {
        set_i(e_Int, tag, &sValue, GetFixSize(sValue));
    }

    void set(tag_t tag, unsigned short usValue)
    {
        set_i(e_Uint, tag, &usValue, GetFixSize(usValue));
    }

    void set(tag_t tag, int iValue)
    {
        set_i(e_Int, tag, &iValue, GetFixSize(iValue));
    }

    void set(tag_t tag, unsigned int uiValue)
    {
        set_i(e_Uint, tag, &uiValue, GetFixSize(uiValue));
    }

    void set(tag_t tag, long lValue)
    {
        set_i(e_Int, tag, &lValue, GetFixSize(lValue));
    }

    void set(tag_t tag, unsigned long ulValue)
    {
        set_i(e_Uint, tag, &ulValue, GetFixSize(ulValue));
    }

    void set(tag_t tag, long long llValue)
    {
        set_i(e_Int, tag, &llValue, GetFixSize(llValue));
    }

    void set(tag_t tag, unsigned long long ullValue)
    {
        set_i(e_Uint, tag, &ullValue, GetFixSize(ullValue));
    }

    void set(tag_t tag, const char* pString)
    {
        
        size_t len = (NULL == pString) ? 0 : ACE_OS::strlen(pString) + 1;
        set_i(e_String, tag, (void*)pString, len);
    }

    void set(tag_t tag, const char* pValue, size_t length)
    {
        set_i(e_Octet, tag, (void*)pValue, length);
    }

    void set(tag_t tag, const Json::Value &v)
    {
        ETLVType t;
        if(v.isArray())
        {
            t = e_Array;
        }
        else
        {
            t = e_Json;
        }

        Json::FastWriter w;
        std::string value = w.write(v);
        size_t len=value.length()+1;
        if(value[value.size()-1]=='\n')
        {            
            value[value.size()-1]=0;
            len=value.length();
        }
        set_i(t,tag,(void*)value.c_str(),len);
    }



    void set_bool(tag_t tag,bool v)
    {
        unsigned char value= 0;
        if(v)
        {
           value = 1;
        }
        set_i(e_Bool, tag, &value, 1);
    }

    void set(tag_t tag, Enum_T v)
    {
        set_i(e_Enum, tag, &v.value, GetFixSize(v.value));
    }
    void set(tag_t tag, Time_T v)
    {
        set_i(e_Time, tag, &v.value, GetFixSize(v.value));
    }

    operator const char*() const
    {
        checkString();
        return pValue_;
    }

    operator char() const
    {
        checkNumerical();
        return ValueCast<char>()(this);
    }
 
    operator unsigned char() const
    {
        checkNumerical();
        return ValueCast<unsigned char>()(this);
    }

    operator short() const
    {
        checkNumerical();
        return ValueCast<short>()(this);
    }

    operator unsigned short() const
    {
        checkNumerical();
        return ValueCast<unsigned short>()(this);
    }
 
    operator int() const
    {
        checkNumerical();
        return ValueCast<int>()(this);
    }

    operator unsigned int() const
    {
        checkNumerical();
        return ValueCast<unsigned int>()(this);
    }

    operator long long() const
    {
        checkNumerical();
        return ValueCast<long long>()(this);
    }

    operator unsigned long long() const
    {
        checkNumerical();
        return ValueCast<unsigned long long>()(this);
    }

    operator bool() const
    {
        checkNumerical();
        return 0 != ValueCast<unsigned long long>()(this);
    }

    ETLVType type(void) const
    {
        return (ETLVType)(usMixTag_ & e_TypeBitMask);
    }

    tag_t tag(void) const
    {
        return (usMixTag_ >> e_TypeBitSize);
    }

    unsigned short length(void) const
    {
        return usLength_;
    }

    const char * value(void) const
    {
        return pValue_;
    }
    
     
    bool getArray(std::list<std::string> &lstID) const
    {
        if(type() == e_Array)
        {
            Json::Reader r;
            char cStrID[4096] = {0};
            
            (void)strncpy_s(cStrID, sizeof(cStrID), value(), sizeof(cStrID)-1);
            bool bStart = false;
            bool bIDStart = false;
            bool bIDOK = false;
            std::string stSingleID;
            size_t iIndex = 0;
            size_t iIDLen = 0;
            char cSingleID[4096] = {0};
            size_t i=0;

            for (; i<strlen(cStrID); i++)
            {
                if (!bStart)
                {
                    bIDStart = false;
                    bStart = (cStrID[i] == '[' || cStrID[i] == ',');
                    continue;
                }

                if (!bIDStart)
                {
                    for (; i<strlen(cStrID); i++)
                    {
                        if (cStrID[i] == '\"' && !bIDStart)
                        {
                            iIndex = i+1;
                            bIDStart = true;
                            continue;
                        }

                        if (cStrID[i] == '\"' && bIDStart)
                        {
                            iIDLen = i - iIndex;
                            bIDStart = false;
                            bIDOK = true;
                            break;
                        }
                    }

                    if (bIDOK)
                    {
                        memset_s(cSingleID, sizeof(cSingleID), 0, sizeof(cSingleID));
                        memcpy_s(cSingleID, sizeof(cSingleID), &cStrID[iIndex], iIDLen);
                        stSingleID = cSingleID;
                        lstID.push_back(stSingleID);
                    }
                    bStart = false;
                }

            }

            return true;
        }
        return false;
    }
    
    

    bool getArray(Json::Value &v) const
    {
        if(type() == e_Array)
        {
            Json::Reader r;
            return r.parse(value(),v);
        }
        return false;
    }

    bool getJson(Json::Value &v) const
    {
        if(type() == e_Json)
        {
            Json::Reader r;
            return r.parse(value(),v);
        }
        return false;
    }

    std::string toString(void);
private:


    template <typename T>
    struct ValueCast
    {
        T cast(const CTLVCell * pCell)
        {
            T v(0);
            if (sizeof(char) == pCell->length())
            {
                v = static_cast<T>((e_Int != pCell->type()) ? (((u_char*)pCell->value())[0])
                    : (pCell->value()[0]));
            }
            else if (sizeof(short) == pCell->length())
            {

                v = static_cast<T>((e_Int !=  pCell->type()) ? (((u_short*)pCell->value())[0])
                    : (((short*)pCell->value())[0]));                
            }
            else if (sizeof(int) >= pCell->length())
            {
                int i(0);
                u_int ui(0);

                char* vptr = (e_Int !=  pCell->type()) ? (char*)(&ui) : (char*)(&i);
                copyByte(pCell->value(), vptr, pCell->length());
                v = static_cast<T>((e_Int != pCell->type()) ? ui : i);
            }
            else
            {
                long long ll(0);
                unsigned long long ull(0);

                char* vptr = (e_Int != pCell->type()) ? (char*)(&ull) : (char*)(&ll);
                copyByte(pCell->value(), vptr, pCell->length());
                v = static_cast<T>((e_Int != pCell->type()) ? ull : ll);
            }

            return v;
        }
        T operator()(const CTLVCell * pCell)
        {
            if (0 == pCell->length())
            {
                return T(0);
            }

            if (sizeof(T) <= pCell->length())
            {
                return *((T*)pCell->value());
            }

            
            return cast(pCell);
        }

    private:
        void copyByte(const char* src, char* dst, size_t length)
        {
            size_t pos = 0;
            do 
            {
                dst[pos] = src[pos];
                pos++;
            } while (pos < length);
        }
    };

private:

    tag_t mixtag(void) const
    {return usMixTag_;}
 
    void set_i(ETLVType type, tag_t tag, const void* value, size_t length);

    void checkString(void) const
    {
        if (e_String != this->type())
        {
            
            
            
            char temp[512] = {0};
            ETLVType ntype = this->type();
            if(ntype >= e_Invalid && ntype <= ETLVType_END)
            {
                
                (void)sprintf_s(temp, sizeof(temp), "CTLVCell isn't string type!tag:%u type:%d",this->tag(),ntype);
            }
            throw CLIException(temp);
            
        }
    }

    void checkNumerical(void) const
    {
        if (e_Uint != this->type()
                && e_Int != this->type()
                && e_Bool != this->type()
                && e_Enum != this->type()
                && e_Time != this->type())
        {
            
            
            char temp[512];
            memset_s(temp, sizeof(temp), 0, sizeof(temp));
            ETLVType ntype = this->type();
            if(ntype >= e_Invalid && ntype <= ETLVType_END)
            {
            
                (void)sprintf_s(temp, sizeof(temp), "CTLVCell isn't string type!tag:%u type:%d",this->tag(),ntype);
            }
            throw CLIException(temp);
            
        }
    }
private:
    tag_t usMixTag_;                
    unsigned short usLength_;       
    char* pValue_;                  
};


class COMMUNICATION_API CTLVCell_Const_Iterator
{
public:

    CTLVCell_Const_Iterator(ACE_DLList_Node* pNode = NULL)
    {
        pNode_ = pNode;
    }

    CTLVCell_Const_Iterator(const CTLVCell_Const_Iterator& other)
    {
        this->pNode_ = other.pNode_;
    }

    virtual ~CTLVCell_Const_Iterator() {}

    CTLVCell_Const_Iterator& operator =(const CTLVCell_Const_Iterator& other)
    {
        if (this == &other)
        {
            return *this;
        }

        this->pNode_ = other.pNode_;
        return *this;
    }

    const CTLVCell& operator*() const
    {
        return *((CTLVCell*)pNode_->item_);
    }

    CTLVCell* const operator->() const
    {
        return ((CTLVCell*)pNode_->item_);
    }

    CTLVCell_Const_Iterator operator++()
    {
        pNode_ = pNode_->next_;
        return *this;
    }

    const CTLVCell_Const_Iterator operator++(int)
    {
        pNode_ = pNode_->next_;
        return *this;
    }

    CTLVCell_Const_Iterator operator--()
    {
        pNode_ = pNode_->prev_;
        return *this;
    }

    const CTLVCell_Const_Iterator operator--(int)
    {
        pNode_ = pNode_->prev_;
        return *this;
    }

    bool operator!=(const CTLVCell_Const_Iterator& other) const
    {
        return (this->pNode_ != other.pNode_);
    }

    bool operator==(const CTLVCell_Const_Iterator& other) const
    {
        return (this->pNode_ == other.pNode_);
    }

protected:
    ACE_DLList_Node* pNode_;
};


class COMMUNICATION_API CTLVCell_Iterator : public CTLVCell_Const_Iterator
{
public:

    CTLVCell_Iterator(ACE_DLList_Node* pNode = NULL)

    {
        pNode_ = pNode; 
    }

    CTLVCell_Iterator(const CTLVCell_Iterator& other)
        : CTLVCell_Const_Iterator(other)
    {}

    ~CTLVCell_Iterator() {}

    CTLVCell_Iterator& operator =(const CTLVCell_Iterator& other)
    {
        if (this == &other)
        {
            return *this;
        }

        this->pNode_ = other.pNode_;
        return *this;
    }
 
    CTLVCell& operator*() const
    {
        return *((CTLVCell*)pNode_->item_);
    }
    
    CTLVCell* operator->() const
    {
        return (CTLVCell*)pNode_->item_;
    }
    
    CTLVCell_Iterator operator++()
    {
        pNode_ = pNode_->next_;
        return *this;
    }
    
    const CTLVCell_Iterator operator++(int)
    {
        pNode_ = pNode_->next_;
        return *this;
    }
    
    CTLVCell_Iterator operator--()
    {
        pNode_ = pNode_->prev_;
        return *this;
    }
    
    const CTLVCell_Iterator operator--(int)
    {
        pNode_ = pNode_->prev_;
        return *this;
    }
    
    bool operator!=(const CTLVCell_Iterator& other) const
    {
        return (this->pNode_ != other.pNode_);
    }
    
    bool operator==(const CTLVCell_Iterator& other) const
    {
        return (this->pNode_ == other.pNode_);
    }
};


class COMMUNICATION_API CS5KRecordV3
{
public:
    friend class CTLVCell_Iterator;
    friend class CTLVCell_Const_Iterator;

    typedef CTLVCell_Iterator iterator;
    typedef CTLVCell_Const_Iterator const_iterator;

    enum
    {
        DEFAULT_TAG_AMOUNT = 32     
    };

    CS5KRecordV3(unsigned long long ullCmd = 0xFFFFFFFFFFFFFFFF,
        size_t size = DEFAULT_TAG_AMOUNT) : hashCell_(size)

    {
        ullCmd_ = ullCmd; 
        void* pData = NULL;
        pHeadNode_ = new ACE_DLList_Node(pData);
        if(pHeadNode_) 
        {
            
            pHeadNode_->next_ = pHeadNode_;
            pHeadNode_->prev_ = pHeadNode_;      
        }
    }
    
    CS5KRecordV3(const CS5KRecordV3& other);

    CS5KRecordV3& operator =(const CS5KRecordV3& other);

    ~CS5KRecordV3()
    {
        try
        {
            clear();
            delete pHeadNode_;
            pHeadNode_ = NULL;
        }catch(...){}
    }

    size_t length() const;
    
    void cmd(unsigned long long ullCmdCode)
    {
        ullCmd_ = ullCmdCode;
    }

    unsigned long long cmd(void) const
    {
        return ullCmd_;
    }

    const CTLVCell& operator [](tag_t tag) const;

    CTLVCell& operator [](tag_t tag);

    iterator begin(void)
    {
        return iterator(pHeadNode_->next_);
    }

    const_iterator begin(void) const
    {
        return const_iterator(pHeadNode_->next_);
    }
 
    iterator end(void)
    {
        return iterator(pHeadNode_);
    }

    const_iterator end(void) const
    {
        return const_iterator(pHeadNode_);
    }


    iterator find_tag(tag_t tag)
    {
        ACE_Hash_Map_Entry<int, ACE_DLList_Node*> *entry = NULL;
        int iFound = hashCell_.find(tag, entry);

        return (0 == iFound) ? iterator(entry->int_id_) : iterator(pHeadNode_);
    }

    
    const_iterator find_tag(tag_t tag) const
    {
        ACE_Hash_Map_Entry<int, ACE_DLList_Node*> *entry = NULL;
        int iFound = hashCell_.find(tag, entry);

        return (0 == iFound) ? const_iterator(entry->int_id_) : const_iterator(pHeadNode_);
    }

    bool append(const CTLVCell* pCell);

    bool delete_tag(tag_t tag);

    bool empty(void)
    {
        return (pHeadNode_->next_ == pHeadNode_);
    }

    void clear(void);

    void dump(std::ostream& str) const;

    unsigned count() const
    {
        return (unsigned int)hashCell_.current_size();
    }
    void remove(tag_t t);

private:

    void clone(const CS5KRecordV3& other);
private:
    
    unsigned long long ullCmd_;
    
    ACE_DLList_Node* pHeadNode_;
    
    ACE_Hash_Map_Manager<int, ACE_DLList_Node*, ACE_Null_Mutex> hashCell_;
};

class CS5KPackageV3;

class COMMUNICATION_API CS5KRecordV3_Const_Iterator
{
public:
    CS5KRecordV3_Const_Iterator(size_t index = (size_t)-1, const CS5KPackageV3* pPkg = NULL)
        : index_(index), pPkg_(pPkg)
    {
        if (NULL == pPkg_)
        {
            
            
            index_ = (size_t)-1;
            
        }
    }

    virtual ~CS5KRecordV3_Const_Iterator() {}

    const CS5KRecordV3& operator*() const;

    CS5KRecordV3* const operator->() const;

    CS5KRecordV3_Const_Iterator& operator =(const CS5KRecordV3_Const_Iterator& other);

    CS5KRecordV3_Const_Iterator operator++()
    {
        plus();
        return *this;
    }

    const CS5KRecordV3_Const_Iterator operator++(int)
    {
        plus();
        return *this;
    }

    CS5KRecordV3_Const_Iterator operator--()
    {
        minus();
        return *this;
    }

    const CS5KRecordV3_Const_Iterator operator--(int)
    {
        minus();
        return *this;
    }

    bool operator!=(const CS5KRecordV3_Const_Iterator& other) const
    {
        return (this->index_ != other.index_);
    }

    bool operator==(const CS5KRecordV3_Const_Iterator& other) const
    {
        return (this->index_ == other.index_);
    }

protected:

    void plus(void);

    void minus(void);

    void checkRecord(void) const;
protected:
    size_t index_;
    const CS5KPackageV3 * pPkg_;
};

class COMMUNICATION_API CS5KRecordV3_Iterator : public CS5KRecordV3_Const_Iterator
{
public:

    CS5KRecordV3_Iterator(size_t index = -1, CS5KPackageV3* pPkg = NULL)
        : CS5KRecordV3_Const_Iterator(index, pPkg)
    {}

    ~CS5KRecordV3_Iterator() {}
    
    CS5KRecordV3& operator*() const;
    
    CS5KRecordV3* operator->() const;
    
    CS5KRecordV3_Iterator& operator =(const CS5KRecordV3_Iterator& other);
    
    CS5KRecordV3_Iterator operator++()
    {
        plus();
        return *this;
    }
    
    const CS5KRecordV3_Iterator operator++(int)
    {
        plus();
        return *this;
    }
    
    CS5KRecordV3_Iterator operator--()
    {
        minus();
        return *this;
    }
    
    const CS5KRecordV3_Iterator operator--(int)
    {
        minus();
        return *this;
    }
    
    bool operator!=(const CS5KRecordV3_Iterator& other) const
    {
        return (this->index_ != other.index_);
    }
    
    bool operator==(const CS5KRecordV3_Iterator& other) const
    {
        return (this->index_ == other.index_);
    }

};
class CS5KRecordV3;

class ICellHideInDumpCondtion
{
public:
    virtual void getHideCellTag(const CS5KRecordV3 &,std::vector<tag_t> &cellNeedHide)=0;
    virtual ~ICellHideInDumpCondtion(){}
};

class COMMUNICATION_API CS5KPackageV3 : public CS5kPkgBase
{
public:
    static ICellHideInDumpCondtion*setCellHideInDumpCondtion(ICellHideInDumpCondtion*);
    static ICellHideInDumpCondtion*getCellHideInDumpCondtion();
    friend class CS5KRecordV3_Iterator;
    friend class CS5KRecordV3_Const_Iterator;

    typedef CS5KRecordV3_Iterator iterator;
    typedef CS5KRecordV3_Const_Iterator const_iterator;

    
    unsigned errorCode();

    CS5KPackageV3() : CS5kPkgBase()
    {}

    CS5KPackageV3(const CS5KPackageV3& other) : CS5kPkgBase(other)
    {
        copy(other);
    }

    CS5KPackageV3& operator=(const CS5KPackageV3& other)
    {
        for (std::vector<CS5KRecordV3*>::iterator it = vMsgPkg_.begin();
            it != vMsgPkg_.end(); ++it)
        {
            delete *it;
            *it = NULL;
        }

        vMsgPkg_.clear();

        copy(other);
        return *this;
    }

    virtual ~CS5KPackageV3()
    {
        try
        {
            clear();
        }catch(...){}
    }

    virtual int decode(unsigned char* pBuf, unsigned int uiBufSize);

    virtual int encode(unsigned char*& pBuf);
    virtual int encode(unsigned char* pBuf,int bufsize);

    virtual void SessionKey(unsigned int uiSessionKey)
    {
        oHead_.uiSessionKey = uiSessionKey;
    }

    virtual unsigned int SessionKey(void) const
    {
        return oHead_.uiSessionKey;
    }

    virtual void result(unsigned int iResult)
    {
        oHead_.uiStatus = iResult;
    }

    virtual unsigned int result(void) const
    {
        return oHead_.uiStatus;
    }

    virtual unsigned int version(void) const
    {
        return oHead_.uiVersion;
    }

    virtual void clear(void);

    virtual void dump(std::ostream& str) const;
  
    bool append(CS5KRecordV3* pRecord)
    {
        if (NULL == pRecord)
        {
            return false;
        }

        vMsgPkg_.push_back(pRecord);
        return true;
    }
    
    size_t count(void) const
    {
        return vMsgPkg_.size();
    }

    CS5KRecordV3& operator [](size_t index)
    {
        if (vMsgPkg_.size() <= index)
        {
            std::stringstream ostr;
            ostr << "record don't exist, index is " << index;
            throw CLIException(ostr.str());
        }

        return *vMsgPkg_[index];
    }

    const CS5KRecordV3& operator [](size_t index) const
    {
        if (vMsgPkg_.size() <= index)
        {
            std::stringstream ostr;
            ostr << "record don't exist, index is " << index;
            throw CLIException(ostr.str());
        }

        return *vMsgPkg_[index];
    }

    iterator begin(void)
    {
        size_t index = vMsgPkg_.empty() ? CS5KPackageV3::INVALID_INDEX : 0;
        return iterator(index, this);
    }

    const_iterator begin(void) const
    {
        size_t index = vMsgPkg_.empty() ? CS5KPackageV3::INVALID_INDEX : 0;
        return const_iterator(index, this);
    }

    iterator end(void)
    {
        return iterator(CS5KPackageV3::INVALID_INDEX, this);
    }

    const_iterator end(void) const
    {
        return const_iterator(CS5KPackageV3::INVALID_INDEX, this);
    }

    iterator find(unsigned long long ullCmd)
    {
        RecordFindIf oCmpFunctor(ullCmd);
        std::vector<CS5KRecordV3*>::iterator it = std::find_if(vMsgPkg_.begin(), vMsgPkg_.end(), oCmpFunctor);
        size_t index = (it == vMsgPkg_.end()) ? CS5KPackageV3::INVALID_INDEX : (it - vMsgPkg_.begin());
        return iterator(index, this);
    }

    const_iterator find(unsigned long long ullCmd) const
    {
        RecordFindIf oCmpFunctor(ullCmd);
        std::vector<CS5KRecordV3*>::const_iterator it = std::find_if(vMsgPkg_.begin(), vMsgPkg_.end(), oCmpFunctor);
        size_t index = (it == vMsgPkg_.end()) ? CS5KPackageV3::INVALID_INDEX : (it - vMsgPkg_.begin());
        return const_iterator(index, this);
    }

    void sort(void)
    {
        std::sort(vMsgPkg_.begin(), vMsgPkg_.end(), RecordCompare);
    }

    const S5KMsgHeadV3& head(void) const
    {
        return oHead_;
    }
    
    S5KMsgHeadV3& head(void)
    {
        return oHead_;
    }

    void nodeID(unsigned short usnodeID)
    {
        oHead_.usNodeID = usnodeID;
    }

    unsigned short nodeID(void) const
    {
        return oHead_.usNodeID;
    }

    void boardID(unsigned short usboardID)
    {
        oHead_.usBoardID = usboardID;
    }

    unsigned short boardID(void) const
    {
        return oHead_.usBoardID;
    }
    void remove(unsigned i)
    {
        if(vMsgPkg_.begin()+(int)i<vMsgPkg_.end())
        {
            delete vMsgPkg_[i];
            vMsgPkg_.erase(vMsgPkg_.begin()+(int)i);
        }
    }
    void moveTo(CS5KPackageV3 &t,int i=-1);
    unsigned encodeLength();

private:

    static bool RecordCompare(const CS5KRecordV3* pR1, const CS5KRecordV3* pR2)
    {
        return (pR1->cmd() < pR2->cmd());
    }

    S5KMsgHeadV3* valid(unsigned char* pBuf, unsigned int uiBufSize);

    unsigned int getRecord(unsigned char* pBuf, unsigned int uiBufSize, CS5KRecordV3*& pRecord);

    unsigned int setRecord(unsigned char* pBuf, const CS5KRecordV3* pRecord);

    void copy(const CS5KPackageV3& other)
    {
        for (std::vector<CS5KRecordV3*>::const_iterator it = other.vMsgPkg_.begin();
            it != other.vMsgPkg_.end(); ++it)
        {
            this->vMsgPkg_.push_back(new CS5KRecordV3(**it));
        }

        this->oHead_ = other.oHead_;
    }

    struct RecordFindIf
    {
        unsigned long long ullCmd_;

        RecordFindIf(unsigned long long ullCmd) : ullCmd_(ullCmd)
        {}

        bool operator ()(const CS5KRecordV3* pRecord)
        {
            return (pRecord->cmd() > ullCmd_);
        }
    };
private:
    
    static const unsigned int INVALID_INDEX = 0xFFFFFFFF;
    
    
    static ACE_Atomic_Op<ACE_Thread_Mutex, unsigned short> oAtomicSerial_;
    

    std::vector<CS5KRecordV3*> vMsgPkg_;    
    S5KMsgHeadV3 oHead_;                    
};


bool COMMUNICATION_API isInvalidInterger(CTLVCell &cell,int len);

enum FINDRES
{
    FINDRES_OK,
    FINDRES_SRC_VALUE_NOT_FOUND,
    FINDRES_SRC_CELL_NOT_EXIST,
    FINDRES_DES_CELL_NOT_EXIST
};
FINDRES COMMUNICATION_API findCellByCell(CS5KPackageV3 &rsp,CTLVCell &ret,
                                         tag_t src_tag,const std::string&src_value,
                                         tag_t des_tag);



#endif 
