# Django 基本命令


本节主要是为了让您了解一些django最基本的命令，请尝试着记住它们，并且多多练习下，特别是标记为红色的那些

打开 Linux 或 MacOS 的 Terminal （终端）直接在 终端中输入这些命令（不是 python 的 shell中）

如果是 windows 用 cmd（开始 搜索 cmd 或者 快捷键 win + R，输入 cmd) 直接在 cmd 上操作。


##1. 新建一个 django project

```sh
django-admin.py startproject project-name
```

一个 project 为一个项目，project-name 项目名称，改成你自己的，要符合Python 的变量命名规则（以下划线或字母开头）

##2. 新建 app
```sh
python manage.py startapp app-name
或 django-admin.py startapp app-name
```

一般一个项目有多个app, 当然通用的app也可以在多个项目中使用。

##3. 同步数据库
```sh
python manage.py syncdb
 
注意：Django 1.7.1及以上的版本需要用以下命令
python manage.py makemigrations
python manage.py migrate
```

这种方法可以创建表，当你在models.py中新增了类时，运行它就可以自动在数据库中创建表了，不用手动创建。

备注：对已有的 models 进行修改，Django 1.7之前的版本的Django都是无法自动更改表结构的，不过有第三方工具 south,详见 Django 数据库迁移 一节。

##4. 使用开发服务器
开发服务器，即开发时使用，一般修改代码后会自动重启，方便调试和开发，但是由于性能问题，建议只用来测试，不要用在生产环境。

```sh
python manage.py runserver

# 当提示端口被占用的时候，可以用其它端口：
python manage.py runserver 8001
python manage.py runserver 9999
（当然也可以kill掉占用端口的进程）
 
# 监听所有可用 ip （电脑可能有一个或多个内网ip，一个或多个外网ip，即有多个ip地址）
python manage.py runserver 0.0.0.0:8000
# 如果是外网或者局域网电脑上可以用其它电脑查看开发服务器
# 访问对应的 ip加端口，比如 http://172.16.20.2:8000
```

##5. 清空数据库
```sh
python manage.py flush
```

此命令会询问是 yes 还是 no, 选择 yes 会把数据全部清空掉，只留下空表。

##6. 创建超级管理员
```sh
python manage.py createsuperuser
# 按照提示输入用户名和对应的密码就好了邮箱可以留空，用户名和密码必填
# 修改 用户密码可以用：
python manage.py changepassword username
```
##7. 导出数据 导入数据

```sh
python manage.py dumpdata appname > appname.json
python manage.py loaddata appname.json
```

关于数据操作 详见：数据导入数据迁移，现在了解有这个用法就可以了。

## 8. Django 项目环境终端
```sh
python manage.py shell
```

如果你安装了 bpython 或 ipython 会自动用它们的界面，推荐安装 bpython。

这个命令和 直接运行 python 或 bpython 进入 shell 的区别是：你可以在这个 shell 里面调用当前项目的 models.py 中的 API，对于操作数据，还有一些小测试非常方便。

## 9. 数据库命令行

```sh
python manage.py dbshell
```

Django 会自动进入在settings.py中设置的数据库，如果是 MySQL 或 postgreSQL,会要求输入数据库用户密码。

在这个终端可以执行数据库的SQL语句。如果您对SQL比较熟悉，可能喜欢这种方式。

##10. 更多命令

```sh
终端上输入 python manage.py 可以看到详细的列表，在忘记子名称的时候特别有用。
```

