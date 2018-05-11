#ifndef __SCONFIG_H__
#define __SCONFIG_H__

#include "config_data.h"
#include <vector>
#include <map>
#include <string>

class SConfig : public ConfigData
{
    
public:
    
    SConfig();
    
    ~SConfig();
    
    int parse();
    
    void clear();
    
public:
    
    std::string _host;
    int _port;
    
    int _zoneid;
    std::string str_zoneid();
    
    std::string _rd_host;
    int _rd_port;
    std::string _rd_password;
    
    std::string _log_host;
    int _log_port;
    
    std::string _http_host; //服务器间通信 地址
    int _http_port;
    
    std::string _facebookurl;
    std::string _appsflyerurl;
    
    //开服时间
    int _year;
    int _month;
    int _day;
    int _hour;
    int _min;
    int _sec;
    
    int _start_timestamp;
    
    std::string data_dir;
    std::string log_file;
    std::string mk_logs;
};

#endif // __SCONFIG_H__

