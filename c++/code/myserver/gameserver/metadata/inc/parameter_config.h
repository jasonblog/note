#ifndef __PARAMETER_CONFIG_H__
#define __PARAMETER_CONFIG_H__

#include <string>
#include <map>
#include <vector>
#include <string>
#include "../../../framework/json/json.h"
#include "server_tool.h"

class ParameterItem
{
    
public:
    
    ParameterItem();
    
    ~ParameterItem();

public:
    const int GetType() const {return _type;}
    const int GetInteger() const {return _integer;}
    const float GetFloater() const { return _floater;}
    const double GetDouble() const { return _doubler;}
    const std::string& GetStr() const {return _str;}
    const std::map<std::string, std::vector<int>* >& GetStrMap() const {return _common;}
    const std::vector<int>& GetVector() const {return _vi;}
    const std::map<int, int>& GetiiMap() const {return _iimap;}
    const std::map<int, float>& GetifMap() const {return _ifmap;}

public:
    int _type;
    
    int _integer;
    
    float _floater;
    
    double _doubler;
    
    std::string _str;
    
    std::map<std::string, std::vector<int>* > _common;
    
    std::vector<int> _vi;
    
    std::map<int, int> _iimap;
    
    std::map<int, float> _ifmap;
};

class ParameterConfig : public CSingleton<ParameterConfig>,public MyJson::Json
{
public:
    
    ParameterConfig();
    
    ~ParameterConfig();
    
    int parse();
    
    void clear();

public:
    const ParameterItem* item(const std::string& key) const;
    const std::map<std::string, ParameterItem*>& GetDatas() const { return _datas;}
private:
    std::map<std::string, ParameterItem*> _datas;
};

template<class ParameterConfig> ParameterConfig* CSingleton<ParameterConfig>::spSingleton = NULL;

#endif // __PARAMETER_CONFIG_H__

