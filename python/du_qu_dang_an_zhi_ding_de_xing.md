# 讀取檔案指定的行



- test_data

```sh
5 10 6
6 20 1
7 30 4
8 40 3
9 23 1
4 13 6
```


```py
#!/usr/bin/python
# -*- coding: utf-8 -*-

f=open("./test_data","r")
lines=f.readlines()
result=[]
for x in lines:
    result.append(x.split(' ')[1])
f.close()
```


