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

#include <list>
#include "msg_package.h"

#define BYTE_COPY(t, s, l) \
	do  \
	{ \
	--(l); \
	((char*)(t))[(l)] = ((char*)(s))[(l)]; \
	} while ((l))


const unsigned int MIN_TLV_SIZE  = 4;
const unsigned int MIN_RECORD_SIZE = 10;

CTLVCell::CTLVCell(const CTLVCell& other) : pValue_(NULL)
{
    if (0 < other.usLength_){
        this->pValue_ = new char[other.usLength_];
        memcpy_s(this->pValue_, other.usLength_, other.pValue_, other.usLength_);
    }

    this->usLength_ = other.usLength_;
    this->usMixTag_ = other.usMixTag_;
}

CTLVCell& CTLVCell::operator =(const CTLVCell& other)
{
    if (this == &other){
        return *this;
    }

    delete [] this->pValue_;
    this->pValue_ = NULL;

    if (0 < other.usLength_){
        this->pValue_ = new char[other.usLength_];
        memcpy_s(this->pValue_, other.usLength_, other.pValue_, other.usLength_);
    }

    this->usLength_ = other.usLength_;
    this->usMixTag_ = other.usMixTag_;
    return *this;
}

void CTLVCell::set_i(ETLVType eType, tag_t usTag, const void* pValue, size_t uiLength)
{
    if (MAX_TAG_VALUE < usTag){
        std::stringstream ostr;
        ostr << "tag is big than MAX_TAG_VALUE, tag is " << usTag;

        throw CLIException(ostr.str());
    }

    delete [] this->pValue_;
    this->pValue_ = NULL;
    this->usLength_ = 0;

    if ((0 < uiLength) && (NULL != pValue)){
        this->usLength_ = (unsigned short)uiLength;
        this->pValue_ = new char[uiLength];
        BYTE_COPY(this->pValue_, pValue, uiLength);
    }

    unsigned int uType = (unsigned int) eType;
    this->usMixTag_ = (usTag << e_TypeBitSize) | uType;
}

std::string CTLVCell::toString(void)
{
    ETLVType etype = this->type();

    if (e_Invalid == etype){
        return std::string("");
    }
    else if (e_Octet == etype){
        return std::string(this->value(), this->length());
    }
    else if (e_String == etype){
        return std::string(this->value());
    }
    else if (e_Uint) {
        std::stringstream ostr;
        unsigned long long ullValue = *this;

        ostr << ullValue;
        return ostr.str();
    }
    return std::string("");


}

bool CS5KRecordV3::append(const CTLVCell* pCell)
{
    if (NULL == pCell){
        return false;
    }

    void * pData = (void*)pCell;
    ACE_DLList_Node* pNode = new ACE_DLList_Node(pData);
    ACE_Hash_Map_Entry<int, ACE_DLList_Node*> *entry = NULL;
    int iFound = hashCell_.find(pCell->tag(), entry);

    if (0 != iFound){
        hashCell_.bind(pCell->tag(), pNode);
    }

    
    ACE_DLList_Node* pRoot = (0 == iFound) ? entry->int_id_->next_ : pHeadNode_;
    pNode->prev_ = pRoot->prev_;
    pNode->next_ = pRoot;
    pRoot->prev_->next_ = pNode;
    pRoot->prev_ = pNode;

    return true;
}

bool CS5KRecordV3::delete_tag(tag_t tag)
{
    ACE_Hash_Map_Entry<int, ACE_DLList_Node*> *entry = NULL;
    int iFound = hashCell_.find(tag, entry);

    if (0 != iFound){
        return true;
    }
    
    ACE_DLList_Node* pRoot = entry->int_id_;
    pRoot->prev_->next_ = pRoot->next_;
    pRoot->next_->prev_ = pRoot->prev_;

    hashCell_.unbind(tag);

    
    delete (CTLVCell*)(pRoot->item_);
    
    delete pRoot;

    return true;
}

void CS5KRecordV3::clear(void)
{
    hashCell_.unbind_all();

    ACE_DLList_Node* pNode = pHeadNode_->next_;

    while (pNode != pHeadNode_){
        
        pNode->prev_->next_ = pNode->next_;
        pNode->next_->prev_ = pNode->prev_;

        
        delete (CTLVCell*)(pNode->item_);
        
        delete pNode;
        pNode = pHeadNode_->next_;
    }
}

CTLVCell& CS5KRecordV3::operator [](tag_t tag)
{
    
    iterator it = this->find_tag(tag);
    if (it == this->end()){
        std::stringstream ostr;
        ostr << "tag don't exist, tag is " << tag<<"rec is:";
        dump(ostr);
        throw CLIException(ostr.str());
    }

    return *it;
}

const CTLVCell& CS5KRecordV3::operator [](tag_t tag) const
{
    
    const_iterator it = this->find_tag(tag); 
    if (it == this->end()){
        std::stringstream ostr;
        ostr << "tag don't exist, tag is " << tag<<" rec is:";
        dump(ostr);
        throw CLIException(ostr.str());
    }

    return *it;
}

void CS5KRecordV3::dump(std::ostream& str) const
{
    static const char* strType[] ={
        "Invalid",
        "Octet",
        "String",
        "Uint",
        "Int",
        "Bool",
        "Enum",
        "Array",
        "Json",
    };

    str << "  {/*record*/" << std::endl;
    str << "    cmd = " << this->ullCmd_ << ", length = " << this->length() << std::endl;

    ACE_DLList_Node* pNode = this->pHeadNode_->next_;
    while (pNode != this->pHeadNode_){
        CTLVCell* pCell = (CTLVCell*)pNode->item_;
        
        CTLVCell::ETLVType nType = pCell->type();
        if(nType < 9){
            str << "    {" << "T,L,V="
                << pCell->tag()
                << ","
                << pCell->length()
                << ","
                << strType[nType]
            << ": ";
        }

        switch (pCell->type()){
        case CTLVCell::e_Octet:{
                str << "{";
                str << std::showbase << std::hex;

                const unsigned char* pcellv = (const unsigned char*)(pCell->value());
                
                for (int i = 0; i < pCell->length(); ++i){
                    unsigned int ui = pcellv[i];
                    str << ui << " ";
                }

                str << std::noshowbase << std::dec;
                str << "} ";
            }
            break;
        case CTLVCell::e_String:
        case CTLVCell::e_Array:
        case CTLVCell::e_Json:{
                str << "\"";
                if (NULL != pCell->value()){
                    str << pCell->value();
                }
                str << "\"";
            }
            break;
        case CTLVCell::e_Int:
            str << (long long)(*pCell);
            break;
        case CTLVCell::e_Uint:
        case CTLVCell::e_Enum:
        case CTLVCell::e_Bool:
            str << (unsigned long long)(*pCell);
            break;
        default:
            str << "null";
        }

        str << "}" << std::endl;
        pNode = pNode->next_;
    }

    str << "  }" << std::endl;
}

void CS5KRecordV3::clone(const CS5KRecordV3& other)
{
    int iRet = 0;
    this->ullCmd_ = other.ullCmd_;
    iRet = this->hashCell_.open(other.hashCell_.total_size());
    if(-1 == iRet){
        return;
    }

    void* pData = NULL;
    pHeadNode_ = new ACE_DLList_Node(pData);
    pHeadNode_->next_ = pHeadNode_->prev_ = pHeadNode_;

    
    for (CS5KRecordV3::const_iterator it = other.begin();it != other.end(); ++it){
        this->append(new CTLVCell(*it));
    }
}

CS5KRecordV3::CS5KRecordV3(const CS5KRecordV3& other)
{
    pHeadNode_ = NULL;
    clone(other);
}

CS5KRecordV3& CS5KRecordV3::operator = (const CS5KRecordV3& other)
{
    if (this != &other){
        clone(other);
    }

    return *this;
}

ACE_Atomic_Op<ACE_Thread_Mutex, unsigned short> CS5KPackageV3::oAtomicSerial_(0);

S5KMsgHeadV3* CS5KPackageV3::valid(unsigned char* pBuf, unsigned int uiBufSize)
{
    if ((NULL == pBuf) || (0 == uiBufSize)){
        return NULL;
    }

    S5KMsgHeadV3* pHead = (S5KMsgHeadV3*)pBuf;
    
    if (uiBufSize != pHead->uiLength + sizeof(S5KMsgHeadV3)){
        return NULL;
    }

    
    if (MAGIC_NUM != pHead->uiMagicNumber){
        return NULL;
    }

    
    if (S5K_MSG_V3 != pHead->uiVersion){
        return NULL;
    }

    return pHead;
}

int CS5KPackageV3::decode(unsigned char* pBuf, unsigned int uiBufSize)
{
    S5KMsgHeadV3* pHead = valid(pBuf, uiBufSize);
    unsigned int pos = 0;

    if (NULL == pHead){
        return -1;
    }

    int iRet = 0;
    while(pos < pHead->uiLength){
        
        unsigned int uiRemainLen = (uiBufSize - sizeof(S5KMsgHeadV3)) - pos;
        if (MIN_RECORD_SIZE > uiRemainLen){
            iRet = -1;
            break;
        }

        CS5KRecordV3* pRecord = NULL;
        if (NULL != pBuf){
            pos += getRecord(pBuf + sizeof(S5KMsgHeadV3) + pos, uiRemainLen, pRecord);
        }

        if (NULL == pRecord){
            iRet = -1;
            break;
        }

        vMsgPkg_.push_back(pRecord);
    }

    if (0 != iRet){
        
        this->clear();
        return -1;
    }
    else{
        oHead_ = *pHead;
    }

    return (int)pos;
}

unsigned int CS5KPackageV3::getRecord(unsigned char* pBuf, unsigned int uiBufSize, CS5KRecordV3*& pRecord)
{
    std::list<CTLVCell*> lstCell;

    
    unsigned long long ullMsgCode = 0;
    unsigned short usRecordLen = 0;
    if (NULL != pBuf){
        ullMsgCode = *((unsigned long long*)pBuf);
        usRecordLen = *((unsigned short*)(pBuf + sizeof(unsigned long long)));
    }
    unsigned short pos = MIN_RECORD_SIZE;
    unsigned char* pread = pBuf;
    size_t iTlvCount = 0;
    pRecord = NULL;

    if ((usRecordLen +  MIN_RECORD_SIZE) > uiBufSize){

        return 0;
    }

    
    while ((pos - MIN_RECORD_SIZE) < usRecordLen){
        if (MIN_TLV_SIZE > (uiBufSize - pos)){

            break;
        }

        
        if (NULL != pread){
            tag_t t = *(tag_t*)(pread + pos);
            pos += sizeof(tag_t);

            if (CTLVCell::ETLVType_END <= (t & CTLVCell::e_TypeBitMask)){

                break;
            }

            unsigned short sDataLen = *((unsigned short*)(pread + pos));
            pos += sizeof(unsigned short);

            if (sDataLen + pos > usRecordLen + MIN_RECORD_SIZE){

                break;
            }

            
            CTLVCell* pCell = new CTLVCell;
            pCell->usMixTag_ = t;
            pCell->usLength_ = sDataLen;

            if (0 < pCell->usLength_){
                pCell->pValue_ = new char[pCell->usLength_];
                BYTE_COPY(pCell->pValue_, pread + pos, sDataLen);
            }

            
            lstCell.push_back(pCell);

            
            pos += pCell->usLength_;

            
            iTlvCount++;
        }
    }
    

    if (pos != (usRecordLen + MIN_RECORD_SIZE)){
        for (std::list<CTLVCell*>::iterator it = lstCell.begin();
            it != lstCell.end(); ++it){
            delete *it;
            *it = NULL;
        }

        return 0;
    }

    iTlvCount = (iTlvCount > (int)(CS5KRecordV3::DEFAULT_TAG_AMOUNT)) ? iTlvCount : (int)(CS5KRecordV3::DEFAULT_TAG_AMOUNT);
    pRecord = new CS5KRecordV3(ullMsgCode, iTlvCount);
    for (std::list<CTLVCell*>::iterator it = lstCell.begin();
        it != lstCell.end(); ++it){
        pRecord->append(*it);
    }

    return pos;
}

unsigned CS5KPackageV3::encodeLength()
{
    unsigned int len = 0;
    std::vector<CS5KRecordV3*>::iterator it;

    
    for (it = vMsgPkg_.begin(); it != vMsgPkg_.end(); ++it){
        len += (*it)->length() + MIN_RECORD_SIZE;
    }
    len += sizeof(S5KMsgHeadV3);
    return len;
}

int CS5KPackageV3::encode(unsigned char* pBuf,int bufsize)
{
    int len = (int)encodeLength();
    if(bufsize <len){
        return -1;
    }
    
    this->oHead_.uiLength = len-sizeof(S5KMsgHeadV3);
    this->oHead_.usSerial = ++oAtomicSerial_;
    this->oHead_.usSerial&=0x7fff;

    errno_t rst = memcpy_s(pBuf, sizeof(S5KMsgHeadV3), &(this->oHead_), sizeof(S5KMsgHeadV3));
    if (rst != EOK){
         return -1; 
    }

    
    unsigned char* pwrite = pBuf + sizeof(S5KMsgHeadV3);
    for (std::vector<CS5KRecordV3*>::iterator it = vMsgPkg_.begin(); it != vMsgPkg_.end(); ++it){
        pwrite += setRecord(pwrite, *it);
    }

    return len;
}

int CS5KPackageV3::encode(unsigned char*& pBuf)
{

    unsigned int len = encodeLength();

    
    if (len <= 0)
        return -1;

    pBuf = new unsigned char[len];

    if (pBuf != NULL)
        return encode(pBuf,(int)len);
    else
        return -1;
}

unsigned int CS5KPackageV3::setRecord(unsigned char* pBuf, const CS5KRecordV3* pRecord)
{
    size_t pos = 0;
    if (NULL != pBuf && NULL != pRecord){
        unsigned short usTypeLen = sizeof(unsigned long long);
        unsigned long long ullCmd = pRecord->cmd();
        
        BYTE_COPY(pBuf, &ullCmd, usTypeLen);
        pos += sizeof(unsigned long long);

        
        unsigned short usLen = (unsigned short)pRecord->length();
        usTypeLen = sizeof(unsigned short);
        
        BYTE_COPY(pBuf + pos, &usLen, usTypeLen);
        pos += sizeof(unsigned short);

        for (CS5KRecordV3::const_iterator it = pRecord->begin();
            it != pRecord->end(); ++it){
            
            tag_t t = (*it).mixtag();
            usTypeLen = sizeof(unsigned short);
            
            BYTE_COPY(pBuf + pos, &t, usTypeLen);
            pos += sizeof(unsigned short);

            
            usLen = (*it).length();
            usTypeLen = sizeof(unsigned short);
            
            BYTE_COPY(pBuf + pos, &usLen, usTypeLen);
            
            pos += sizeof(unsigned short);

            
            if (0 < usLen){
                
                BYTE_COPY(pBuf + pos, (*it).value(), usLen);
            }
            pos += (*it).length();
        }
    }
    return pos;
}

void CS5KPackageV3::clear(void)
{
    for (std::vector<CS5KRecordV3*>::iterator it = vMsgPkg_.begin();it != vMsgPkg_.end(); ++it){
        delete (*it);
        *it = NULL;
    }

    vMsgPkg_.clear();
}

size_t CS5KRecordV3::length() const
{
    size_t len = 0;
    ACE_DLList_Node* pNode = pHeadNode_->next_;

    while (pNode != pHeadNode_){
        CTLVCell* pCell = (CTLVCell*)(pNode->item_);
        
        len += pCell->length() + sizeof(int);
        pNode = pNode->next_;
    }
    return len;
}

CS5KRecordV3& CS5KRecordV3_Iterator::operator*() const
{
    checkRecord();
    return *(pPkg_->vMsgPkg_[index_]);
}

CS5KRecordV3* CS5KRecordV3_Iterator::operator->() const
{
    checkRecord();
    return (NULL != pPkg_) ? (pPkg_->vMsgPkg_[index_]):NULL;
}

CS5KRecordV3_Iterator& CS5KRecordV3_Iterator::operator =(const CS5KRecordV3_Iterator& other)
{
    if (this == &other){
        return *this;
    }

    this->index_ = other.index_;
    this->pPkg_ = other.pPkg_;

    return *this;
}

const CS5KRecordV3& CS5KRecordV3_Const_Iterator::operator*() const
{
    checkRecord();
    return *(pPkg_->vMsgPkg_[index_]);
}

CS5KRecordV3* const CS5KRecordV3_Const_Iterator::operator->() const
{
    checkRecord();
    return (NULL != pPkg_) ? (pPkg_->vMsgPkg_[index_]) : NULL;
}

CS5KRecordV3_Const_Iterator& CS5KRecordV3_Const_Iterator::operator =(const CS5KRecordV3_Const_Iterator& other)
{
    if (this == &other){
        return *this;
    }

    this->index_ = other.index_;
    this->pPkg_ = other.pPkg_;

    return *this;
}

void CS5KRecordV3_Const_Iterator::plus(void)
{
    if (CS5KPackageV3::INVALID_INDEX == index_){
        return;
    }

    ++index_;

    
    if (NULL != pPkg_){
        if (index_ >= pPkg_->vMsgPkg_.size()){
            index_  = CS5KPackageV3::INVALID_INDEX;
        }
    }
}

void CS5KRecordV3_Const_Iterator::minus(void)
{
    if (0 == index_){
        index_ = CS5KPackageV3::INVALID_INDEX;
    }
    else if (CS5KPackageV3::INVALID_INDEX == index_){
        if (NULL != pPkg_){
            index_ = pPkg_->vMsgPkg_.size() - 1;
        }
    }
    else{
        --index_;
    }
}

void CS5KRecordV3_Const_Iterator::checkRecord(void) const
{
    if ((CS5KPackageV3::INVALID_INDEX == index_)
        || (NULL == pPkg_) || (index_ >= pPkg_->vMsgPkg_.size())){
        std::stringstream ostr;
        ostr << "record don't exist, index is " << index_;
        throw CLIException(ostr.str());
    }
}

void CS5KPackageV3::dump(std::ostream& str) const
{
    str << "{/*package*/" << std::endl;
    str << "  version     = " << this->oHead_.uiVersion << std::endl;
    str << "  result      = " << this->oHead_.uiStatus << std::endl;
    str << "  session key = " << this->oHead_.uiSessionKey << std::endl;
    str << "  node id     = " << this->oHead_.usNodeID << std::endl;
    str << "  board id    = " << this->oHead_.usBoardID << std::endl;
    str << "  serial      = " << this->oHead_.usSerial << std::endl;
    str << "  usReserved  = " << this->oHead_.usReserved <<std::endl;
    str << "  length      = " << this->oHead_.uiLength << std::endl;

    for (std::vector<CS5KRecordV3*>::const_iterator it = vMsgPkg_.begin();
        it != vMsgPkg_.end(); ++it){
        (*it)->dump(str);
    }
    str << "}" << std::endl;
}

tag_t TLV_RETURN_CODE = 4095; 

unsigned CS5KPackageV3::errorCode()
{
     /*如果查询返回失败，则抛出异常*/
    unsigned retCode = result();

    if (0 != retCode){
        return retCode;
    }

    if(!vMsgPkg_.empty()){
        
        CS5KRecordV3::const_iterator rit = vMsgPkg_.back()->find_tag(TLV_RETURN_CODE); 
        if(rit != vMsgPkg_.back()->end()){
            return (unsigned)(*rit);
        }
    }
    return 0;
}

bool isInvalidInterger(CTLVCell &cell,int len)
{
    if(len <= 0 || len > 8){
        return false;
    }
    if(cell.type() == CTLVCell::e_Uint){
        
        unsigned long long v = cell;
        if ( ( ( (unsigned long long) -1) >> (64-len * 8 ) ) == v){
            return true;
        }
    }
    else if(cell.type() == CTLVCell::e_Int){
        
        unsigned long long v = cell;
        unsigned int rightVal = 65 - len * 8;
        if (( ( (unsigned long long) -1) >> rightVal ) == v){
            return true;
        }
    }
    return false;
}

FINDRES findCellByCell(CS5KPackageV3 &rsp,CTLVCell &ret,
                       tag_t src_tag,const std::string&src_value,
                       tag_t des_tag)
{
    std::ostringstream oss;
    for(unsigned i = 0; i < rsp.count(); ++i){
       
         CS5KRecordV3::iterator it = rsp[i].find_tag(src_tag);
        if(it == rsp[i].end()){
            oss<<"findCellByCell can't find src cell "<<src_tag<<". in msg:";
            rsp.dump(oss);
            return FINDRES_SRC_CELL_NOT_EXIST;
        }
        if( it->toString() == src_value){

            
            CS5KRecordV3::iterator itdes=rsp[i].find_tag(des_tag); 
            if(itdes == rsp[i].end()){
                oss<<"findCellByCell can't find des cell "<<des_tag<<". in msg:";
                rsp.dump(oss);
                return FINDRES_DES_CELL_NOT_EXIST;
            }
            else{
                ret= *itdes;
                return FINDRES_OK;
            }
        }
    }
    return FINDRES_SRC_VALUE_NOT_FOUND;
}

static ICellHideInDumpCondtion *g_cellHideInDumpCond=0;
ICellHideInDumpCondtion*CS5KPackageV3::setCellHideInDumpCondtion(ICellHideInDumpCondtion*n)
{
    ICellHideInDumpCondtion * o=g_cellHideInDumpCond;
    g_cellHideInDumpCond = n;
    return o;
}

ICellHideInDumpCondtion*CS5KPackageV3::getCellHideInDumpCondtion()
{
    return g_cellHideInDumpCond;
}

