/*******************************************************************************
 * Project:  neb
 * @file     CJsonObject.hpp
 * @brief    Json
 * @author   bwarliao
 * @date:    2014-7-16
 * @note
 * Modify history:
 ******************************************************************************/

#ifndef CJSONOBJECT_HPP_
#define CJSONOBJECT_HPP_

#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <string>
#include <map>
#include <list>
#ifdef __cplusplus
extern "C" {
#endif
#include "cJSON.h"
#ifdef __cplusplus
}
#endif
// typedef int int;
// typedef unsigned int unsigned int;
// typedef long long long long;
// typedef unsigned long long unsigned long long;

namespace neb
{

class CJsonObject
{
public:     // method of ordinary json object or json array
    CJsonObject();
    CJsonObject(const std::string& strJson);
    CJsonObject(const CJsonObject* pJsonObject);
    CJsonObject(const CJsonObject& oJsonObject);
    virtual ~CJsonObject();

    CJsonObject& operator=(const CJsonObject& oJsonObject);
    bool operator==(const CJsonObject& oJsonObject) const;
    bool Parse(const std::string& strJson);
    void Clear();
    bool IsEmpty() const;
    bool IsArray() const;
    std::string ToString() const;
    std::string ToFormattedString() const;
    const std::string& GetErrMsg() const
    {
        return(m_strErrMsg);
    }

public:     // method of ordinary json object
    bool AddEmptySubObject(const std::string& strKey);
    bool AddEmptySubArray(const std::string& strKey);
    bool GetKey(std::string& strKey);
    void ResetTraversing();
    CJsonObject& operator[](const std::string& strKey);
    std::string operator()(const std::string& strKey) const;
    bool Get(const std::string& strKey, CJsonObject& oJsonObject) const;
    bool Get(const std::string& strKey, std::string& strValue) const;
    bool Get(const std::string& strKey, int& iValue) const;
    bool Get(const std::string& strKey, unsigned int& uiValue) const;
    bool Get(const std::string& strKey, long long& llValue) const;
    bool Get(const std::string& strKey, unsigned long long& ullValue) const;
    bool Get(const std::string& strKey, bool& bValue) const;
    bool Get(const std::string& strKey, float& fValue) const;
    bool Get(const std::string& strKey, double& dValue) const;
    bool IsNull(const std::string& strKey) const;
    bool Add(const std::string& strKey, const CJsonObject& oJsonObject);
    bool Add(const std::string& strKey, const std::string& strValue);
    bool Add(const std::string& strKey, int iValue);
    bool Add(const std::string& strKey, unsigned int uiValue);
    bool Add(const std::string& strKey, long long llValue);
    bool Add(const std::string& strKey, unsigned long long ullValue);
    bool Add(const std::string& strKey, bool bValue, bool bValueAgain);
    bool Add(const std::string& strKey, float fValue);
    bool Add(const std::string& strKey, double dValue);
    bool AddNull(const std::string& strKey);    // add null like this:   "key":null
    bool Delete(const std::string& strKey);
    bool Replace(const std::string& strKey, const CJsonObject& oJsonObject);
    bool Replace(const std::string& strKey, const std::string& strValue);
    bool Replace(const std::string& strKey, int iValue);
    bool Replace(const std::string& strKey, unsigned int uiValue);
    bool Replace(const std::string& strKey, long long llValue);
    bool Replace(const std::string& strKey, unsigned long long ullValue);
    bool Replace(const std::string& strKey, bool bValue, bool bValueAgain);
    bool Replace(const std::string& strKey, float fValue);
    bool Replace(const std::string& strKey, double dValue);
    bool ReplaceWithNull(const std::string& strKey);    // replace value with null

public:     // method of json array
    int GetArraySize();
    CJsonObject& operator[](unsigned int uiWhich);
    std::string operator()(unsigned int uiWhich) const;
    bool Get(int iWhich, CJsonObject& oJsonObject) const;
    bool Get(int iWhich, std::string& strValue) const;
    bool Get(int iWhich, int& iValue) const;
    bool Get(int iWhich, unsigned int& uiValue) const;
    bool Get(int iWhich, long long& llValue) const;
    bool Get(int iWhich, unsigned long long& ullValue) const;
    bool Get(int iWhich, bool& bValue) const;
    bool Get(int iWhich, float& fValue) const;
    bool Get(int iWhich, double& dValue) const;
    bool IsNull(int iWhich) const;
    bool Add(const CJsonObject& oJsonObject);
    bool Add(const std::string& strValue);
    bool Add(int iValue);
    bool Add(unsigned int uiValue);
    bool Add(long long llValue);
    bool Add(unsigned long long ullValue);
    bool Add(int iAnywhere, bool bValue);
    bool Add(float fValue);
    bool Add(double dValue);
    bool AddNull();   // add a null value
    bool AddAsFirst(const CJsonObject& oJsonObject);
    bool AddAsFirst(const std::string& strValue);
    bool AddAsFirst(int iValue);
    bool AddAsFirst(unsigned int uiValue);
    bool AddAsFirst(long long llValue);
    bool AddAsFirst(unsigned long long ullValue);
    bool AddAsFirst(int iAnywhere, bool bValue);
    bool AddAsFirst(float fValue);
    bool AddAsFirst(double dValue);
    bool AddNullAsFirst();     // add a null value
    bool Delete(int iWhich);
    bool Replace(int iWhich, const CJsonObject& oJsonObject);
    bool Replace(int iWhich, const std::string& strValue);
    bool Replace(int iWhich, int iValue);
    bool Replace(int iWhich, unsigned int uiValue);
    bool Replace(int iWhich, long long llValue);
    bool Replace(int iWhich, unsigned long long ullValue);
    bool Replace(int iWhich, bool bValue, bool bValueAgain);
    bool Replace(int iWhich, float fValue);
    bool Replace(int iWhich, double dValue);
    bool ReplaceWithNull(int iWhich);      // replace with a null value

private:
    CJsonObject(cJSON* pJsonData);

private:
    cJSON* m_pJsonData;
    cJSON* m_pExternJsonDataRef;
    cJSON* m_pKeyTravers;
    std::string m_strErrMsg;
    std::map<unsigned int, CJsonObject*> m_mapJsonArrayRef;
    std::map<std::string, CJsonObject*> m_mapJsonObjectRef;
};

}

#endif /* CJSONHELPER_HPP_ */
