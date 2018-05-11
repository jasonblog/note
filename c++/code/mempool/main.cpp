//
//  main.cpp
//  Demo
//
//  Created by 杜国超 on 17/6/22.
//  Copyright © 2017年 杜国超. All rights reserved.
//

#include <vector>
#include <iostream>
#include <string>
#include "memsafety.h"

using namespace std;

class Demo
{
public:
    Demo()
    {
        name = "";
        age = 0;
    }
    
    void init(string _name,int _age)
    {
        name = _name;
        age = _age;
    }
    
    string GetName() {return name;}
    int GetAge()  {return age;}
private:
    string name;
    int age;
};

int main()
{
    CMemoryPool<Demo> memPool;
    //初始化内存池大小
    memPool.Create(5);
    std::vector<Demo*> demoArray;
    
    for (int i = 0; i< 10;i++) {
        Demo *demo = memPool.Alloc();
        demo->init(to_string(i), i);
        demoArray.push_back(demo);
    }
    
    for (int i = 0; i< 10;i++) {
        std::cout<< "name:" << demoArray[i]->GetName() << "   age:" << demoArray[i]->GetAge() << std::endl;
        memPool.Free(demoArray[i]);
    }
    
    memPool.Destroy();
    std::cout << "Hello word" <<  std::endl;
}
