//
//  json.h
//  读取json文件
//  Created by dguco on 17-2-16.
//  Copyright © 2016年 DGuco. All rights reserved.
//

#ifndef _JSON_H_
#define _JSON_H_

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
using rapidjson::Value;

namespace MyJson {
    class Json {
    public:
        Json();
        virtual ~Json();
        int LoadFromFile(const std::string &filename);
        virtual int Parse();
        virtual void Clear();
        int Reload();

    protected:
        rapidjson::Document m_Obj;
        std::string m_sFilename;
    };
}
#endif //_JSON_H_