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

#ifndef _CLI_EXCEPTION_H_
#define _CLI_EXCEPTION_H_

#include <string>
#include <exception>

class  CLIException: public std::exception
{
public:
    CLIException(const std::string& strExp): m_Exp( strExp)
    {
    }

    CLIException(const char* pExp):m_Exp(pExp)
    {

    }
    CLIException()
    {

    }
    virtual const char* what() const throw() 
    {
        return m_Exp.c_str();
    }

    virtual ~CLIException() throw()
    {
    }

private:
    std::string m_Exp;
};


#endif //_CLI_EXCEPTION_H_
