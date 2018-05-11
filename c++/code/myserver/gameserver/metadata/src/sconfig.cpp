#include "sconfig.h"

bool splitLogsFile(const std::string& str, std::string& sLogFile)
{
    std::vector<std::string> list;
    std::string::size_type pos;
    int size=str.size();
    for(int i = 0; i<size; ++i)
    {
        pos = str.find('/', i);
        if(pos < size)
        {
            std::string s = str.substr(i, pos-i);
            list.push_back(s);
            i = pos;
        }
        else
        {
            break;
        }
    }
    
    for (int i = 0; i < list.size(); ++i)
    {
        sLogFile += list[i];
        sLogFile += '/';
    }
    
    return true;
}

SConfig::SConfig()
{
    _host.clear();
    _port = 0;
    _zoneid = 0;
}

SConfig::~SConfig()
{
    _host.clear();
    _port = 0;
    _zoneid = 0;
}

std::string SConfig::str_zoneid()
{
    return std::to_string(_zoneid);
}

int SConfig::parse()
{
    auto obj = m_obj.ToObject();
    
    data_dir = obj["data_dir"].ToString();
    log_file = obj["log_file"].ToString();
    
    if (log_file.length() <= 0)
    {
        return -1;
    }
    
    mk_logs.clear();
    splitLogsFile(log_file, mk_logs);
    
    if (mk_logs.length() <= 0)
    {
        return -1;
    }
    
    auto host = obj["host"].ToString();
    auto port = obj["port"].ToInt();
    auto zoneid = obj["zoneid"].ToInt();
    _host = host;
    _port = port;
    _zoneid = zoneid;
    
    auto redis_server = obj["redis_server"].ToObject();
    _rd_host = redis_server["host"].ToString();
    _rd_port = redis_server["port"].ToInt();
    _rd_password = redis_server["password"].ToString();
    
    auto tlog_server = obj["tlog_server"].ToObject();
    _log_host = tlog_server["host"].ToString();
    _log_port = tlog_server["port"].ToInt();
    
    _http_host = obj["http_host"].ToString();
    _http_port = obj["http_port"].ToInt();
    
    std::string start_server = obj["start_server"].ToString();
    
    std::string year = start_server.substr(0,4);
    std::string month = start_server.substr(5,2);
    std::string day = start_server.substr(8,2);
    std::string hour = start_server.substr(11,2);
    std::string min = start_server.substr(14,2);
    std::string sec = start_server.substr(17,2);
    _year = atoi(year.c_str());
    _month = atoi(month.c_str());
    _day = atoi(day.c_str());
    _hour = atoi(hour.c_str());
    _min = atoi(min.c_str());
    _sec = atoi(sec.c_str());
    
    struct tm xtm;
    xtm.tm_year = _year-1900;
    xtm.tm_mon = _month-1;
    xtm.tm_mday = _day;
    xtm.tm_hour = _hour;
    xtm.tm_min = _min;
    xtm.tm_sec = _sec;
    _start_timestamp = mktime(&xtm);
    _facebookurl = obj["event_server"].ToString();
    
    return 0;
}

void SConfig::clear()
{
    _host.clear();
    _port = 0;
}
