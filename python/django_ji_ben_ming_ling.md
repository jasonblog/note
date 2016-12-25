# Django 基本命令


本節主要是為了讓您瞭解一些django最基本的命令，請嘗試著記住它們，並且多多練習下，特別是標記為紅色的那些

打開 Linux 或 MacOS 的 Terminal （終端）直接在 終端中輸入這些命令（不是 python 的 shell中）

如果是 windows 用 cmd（開始 搜索 cmd 或者 快捷鍵 win + R，輸入 cmd) 直接在 cmd 上操作。


##1. 新建一個 django project

```sh
django-admin.py startproject project-name
```

一個 project 為一個項目，project-name 項目名稱，改成你自己的，要符合Python 的變量命名規則（以下劃線或字母開頭）

##2. 新建 app
```sh
python manage.py startapp app-name
或 django-admin.py startapp app-name
```

一般一個項目有多個app, 當然通用的app也可以在多個項目中使用。

##3. 同步數據庫
```sh
python manage.py syncdb
 
注意：Django 1.7.1及以上的版本需要用以下命令
python manage.py makemigrations
python manage.py migrate
```

這種方法可以創建表，當你在models.py中新增了類時，運行它就可以自動在數據庫中創建表了，不用手動創建。

備註：對已有的 models 進行修改，Django 1.7之前的版本的Django都是無法自動更改表結構的，不過有第三方工具 south,詳見 Django 數據庫遷移 一節。

##4. 使用開發服務器
開發服務器，即開發時使用，一般修改代碼後會自動重啟，方便調試和開發，但是由於性能問題，建議只用來測試，不要用在生產環境。

```sh
python manage.py runserver

# 當提示端口被佔用的時候，可以用其它端口：
python manage.py runserver 8001
python manage.py runserver 9999
（當然也可以kill掉佔用端口的進程）
 
# 監聽所有可用 ip （電腦可能有一個或多個內網ip，一個或多個外網ip，即有多個ip地址）
python manage.py runserver 0.0.0.0:8000
# 如果是外網或者局域網電腦上可以用其它電腦查看開發服務器
# 訪問對應的 ip加端口，比如 http://172.16.20.2:8000
```

##5. 清空數據庫
```sh
python manage.py flush
```

此命令會詢問是 yes 還是 no, 選擇 yes 會把數據全部清空掉，只留下空表。

##6. 創建超級管理員
```sh
python manage.py createsuperuser
# 按照提示輸入用戶名和對應的密碼就好了郵箱可以留空，用戶名和密碼必填
# 修改 用戶密碼可以用：
python manage.py changepassword username
```
##7. 導出數據 導入數據

```sh
python manage.py dumpdata appname > appname.json
python manage.py loaddata appname.json
```

關於數據操作 詳見：數據導入數據遷移，現在瞭解有這個用法就可以了。

## 8. Django 項目環境終端
```sh
python manage.py shell
```

如果你安裝了 bpython 或 ipython 會自動用它們的界面，推薦安裝 bpython。

這個命令和 直接運行 python 或 bpython 進入 shell 的區別是：你可以在這個 shell 裡面調用當前項目的 models.py 中的 API，對於操作數據，還有一些小測試非常方便。

## 9. 數據庫命令行

```sh
python manage.py dbshell
```

Django 會自動進入在settings.py中設置的數據庫，如果是 MySQL 或 postgreSQL,會要求輸入數據庫用戶密碼。

在這個終端可以執行數據庫的SQL語句。如果您對SQL比較熟悉，可能喜歡這種方式。

##10. 更多命令

```sh
終端上輸入 python manage.py 可以看到詳細的列表，在忘記子名稱的時候特別有用。
```

