# python


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
f=open("./test_data","r")
lines=f.readlines()
result=[]
for x in lines:
    result.append(x.split(' ')[1])
f.close()
```


