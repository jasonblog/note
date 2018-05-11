//
//  config_manage.h
//  cserver
//  配置表 管理类
//  Created by zhaolong on 16/9/8.
//
//

#ifndef config_manage_h
#define config_manage_h

#include "config_data.h"

class ConfigManage
{
public:
    ConfigManage(){};
    ~ConfigManage(){};
    
public:
    static int load();      //加载所有配置
    static int reload();    //重新加载配置    有热更配置的就写 没有需要就不写
    
private:
    static int check();     //检测配置内容正确性 需要的配置 就写 不需要的就不写
    
    
};

#endif /* config_manage_h */
