# threadpool
基於c++14實現輕便的線程池
# 注意：
1 首先升級c++至支持c++14的版本 </br>
2 編譯時不要引入threapool.cpp文件，比如：我的cmake文件並沒有包含此文件，由於模板函數的存在，.h文件中引入了此文件#include"threadpool.cpp",或者把兩個
  文件合併成一個文件（把.cpp文件中的內容copy到.h中）
# 實現原理很簡單，這裡不再詳述
