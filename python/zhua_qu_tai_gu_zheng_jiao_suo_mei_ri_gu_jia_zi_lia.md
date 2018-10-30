# 抓取台股證交所每日股價資料，儲存到MongoDB


## 1. MongoDB安装

```sh
sudo apt-get install mongodb
```

###查看是否安装成功，查看版本

```sh
mongo -version
```

### 启动/关闭 mongoDB数据库服务的命令

```sh
service mongodb start
service mongodb stop
```

### mongo启动shell模式

```sh
mongo
```


## 2. python调用mongo的驱动安装

### 安装pymongo驱动

```sh
pip install pymongo
```

### 更新pymongo驱动

```sh
pip install --upgrade pip
```

## 3. 测试


```py
#!/usr/bin/env python
# -*- coding:utf-8 -*-

from pymongo import MongoClient

conn = MongoClient('127.0.0.1', 27017)
db = conn.mydb  
my_set = db.test_set

my_set.insert({"name":"joe001","age":3})
my_set.insert({"name":"joe002","age":4})
my_set.insert({"name":"joe003","age":5})

for i in my_set.find():
    print(i)

```


## 好用的 MongoDB GUI manager

```sh
https://robomongo.org/
```

### Robomongo — 好用的 MongoDB GUI manager

```sh
https://medium.com/@wilsonhuang/robomongo-%E5%A5%BD%E7%94%A8%E7%9A%84-mongodb-gui-manager-87508da806e5

```


