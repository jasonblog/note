//
//  drop_config.h
//  Created by mac on 16/12/10.
//  @author: DGuco
//  掉落
//
//


#ifndef __DROP_CONFIG_H__
#define __DROP_CONFIG_H__

#include "config_data.h"

#include <map>
#include <vector>
#include <string>

//掉落配置表
class DropItem
{
    
public:
    DropItem();
    ~DropItem();
    void reset();
    
    
public:
    int miId;            //ID
    int miNum;           //掉落数量
    int isRepeat;        //是否重复凋落
    
    std::map<std::string,std::vector<int>> mmRandomItem; //条件
};

class DropConfig : public ConfigData
{
public:
    DropConfig();
    
    ~DropConfig();
    
    int parse();
    
    void clear();
    
    bool check();
    void drop(int iId,std::map<int,std::vector<int>> & dropResult);
    
    DropItem* item(int key);
    
public:
    
    std::map<int, DropItem* > _datas;
};

#endif // __SIXDAY_CONFIG_H__