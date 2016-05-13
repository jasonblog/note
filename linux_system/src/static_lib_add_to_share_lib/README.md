g++ -c static.cpp // 生成static.o
ar -r libstatic.a static.o // 生成靜態庫libstatic.a
g++ -c -fPIC shared.cpp // 生成shared.o
g++ -shared shared.o -o libshared.so // 生成動態庫libshared.so


g++ test.cpp  -L. -lshared -lstatic -o test.exe // link libshared.so 到test.exe中

This is shared_print function
This is static_print function
