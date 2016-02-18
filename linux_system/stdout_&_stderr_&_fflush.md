# stdout & stderr & fflush()


```c
全緩衝:buffer滿了才執行fflush()
行緩衝:buffer滿了or碰到換行字元才執行fflush()
無緩衝:不管buffer有沒有滿都執行fflush()
cout使用stdout stream
cerr跟clog使用stderr stream

console下
         標準    windows   linux
stdout|行/無緩衝|全緩衝  |行緩衝   |
stderr|行/無緩衝|全緩衝  |無緩衝   |

非console(即重定向)
         標準     windows  linux
stdout|全緩衝   |全緩衝  |全緩衝   |
stderr|行/無緩衝|全緩衝  |無緩衝   |
```