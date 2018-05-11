# threadpool
基于c++14实现轻便的线程池
# 注意：
1 首先升级c++至支持c++14的版本 </br>
2 编译时不要引入threapool.cpp文件，比如：我的cmake文件并没有包含此文件，由于模板函数的存在，.h文件中引入了此文件#include"threadpool.cpp",或者把两个
  文件合并成一个文件（把.cpp文件中的内容copy到.h中）
# 实现原理很简单，这里不再详述
