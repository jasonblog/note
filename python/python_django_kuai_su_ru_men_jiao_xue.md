# Python Django 快速入門教學


##什麼是 Django？
Django 是一個使用 Python 開發 Web 的應用程式框架（Frameowork），相對於 Flask 這種 micro-framework，Django 屬於一種大而全的 MVC 框架（實際上是 MTV），讓開發 Web 應用程式可以快速打造網路應用程式，而不用重複造輪子。

簡單來說像 Django 這類的 Web Frameowork，就是幫你把大部分的程式架構都建構好，等你填你需要的應用邏輯進去。Framework 會負責在合適的時機呼叫你寫的程式，而不是讓你自行呼叫合適的函式（function），以搭建程式本身。


## 環境建置與設定

```sh
sudo pip3 install virtualenv

mkdir opencook
cd opencook

// -m 代表執行模組或函式庫 
$ virtualenv -p python3 opencook_venv

source opencook/bin/activate
(opencook_venv)$~/

deactivate
```

# 開始 Django 專案

```sh
$ django-admin.py startproject opencook
$ cd opencook

// 同步數據庫
python manage.py syncdb
注意：Django 1.7.1及以上的版本需要用以下命令
python manage.py makemigrations
python manage.py migrate



Operations to perform:
  Apply all migrations: admin, auth, contenttypes, sessions
Running migrations:
  Applying contenttypes.0001_initial... OK
  Applying auth.0001_initial... OK
  Applying admin.0001_initial... OK
  Applying admin.0002_logentry_remove_auto_add... OK
  Applying contenttypes.0002_remove_content_type_name... OK
  Applying auth.0002_alter_permission_name_max_length... OK
  Applying auth.0003_alter_user_email_max_length... OK
  Applying auth.0004_alter_user_username_opts... OK
  Applying auth.0005_alter_user_last_login_null... OK
  Applying auth.0006_require_contenttypes_0002... OK
  Applying auth.0007_alter_validators_add_error_messages... OK
  Applying auth.0008_alter_user_username_max_length... OK
  Applying sessions.0001_initial... OK
```

## 資料夾結構

```sh
 tree
.
├── db.sqlite3
├── manage.py
└── opencook
    ├── __init__.py
    ├── __init__.pyc
    ├── settings.py
    ├── settings.pyc
    ├── urls.py
    ├── urls.pyc
    └── wsgi.py
```

## Run

```sh
python manage.py runserver 1111

Starting development server at http://127.0.0.1:1111/
Quit the server with CONTROL-C.
```

##Django 的 Management commands

```sh
django-admin.py startproject ：建立初始化 Django 專案
python manage.py -h ：查看 Django commands 的使用方法
python manage.py runserver：啟動開發用伺服器
python manage.py startapp ：新增 Django app
```

## 建立 Django application（app）

```sh
python manage.py startapp recipes
```

## 將新增的 Django app 加入設定檔

```sh
# mysite/settings.py

...

# Application definition

INSTALLED_APPS = (
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'recipes',
)
```
##是 MVC？ 還是 MTV？
現在許多的 Web 框架多半標榜是 MVC 框架，由 Model 負責資料層，View 負責視覺，Controller 負責商業邏輯的部份。雖然 Django 也是類似於 MVC 大而全的框架，但嚴格來說我們會把 Django 視為 MTV，也就是 Model、View、Template，在 Django 中 View 類似於 Controller 功能，而 Template 則處理 View 視覺部分。

##Views and URLconfs
在 Django 中會透過正規表達式來處理 URL Match Routing 的部份，並透過 Views 載入對應的處理函數。

##Templates
在 Django 中 Templates 類似於 MVC 中的 View，主要負責使用者介面。

##Models
與大部分 MVC 框架差不多，在 Django 中 Model 主要是處理資料層的資料。

##Admin
Django 有一個比較方便的功能就是內建的 Admin，可以讓你簡單就建立一個可以管理後臺的系統，在一般情況下通常堪用了。

##Django ORM
ORM 的全名是 Object-Relational Mapping，係指將關聯式資料庫映射至物件導向的資料抽象化技術，可以讓你比較方便的去操作資料庫，當然如果是複雜關聯式資料庫操作還是要使用到 SQL 語法。

##表單互動
- 用 HTML 刻個 form 表單
- 處理 HTTP POST request
- 驗證表單欄位內容是否正確
- 把確認過的資料存進 database 之中

```html
def signup(request):
    if request.user.is_authenticated(): 
        return HttpResponseRedirect('/')

    if request.method == 'POST':
        form = UserCreationForm(request.POST)
        if form.is_valid():
            user = form.save()
            return HttpResponseRedirect('/accounts/login')

        return render(request, 'accounts/signup.html', locals())
    else:
    	form = UserCreationForm()
    	return render(request, 'accounts/signup.html', locals())
    	
```

```html
{% extends 'extends/base.html' %}

{% block content %}
<br>
<br>
<br>
<br>
<div class="container">
	<h1>註冊新帳號</h1>
	<form action="/contact/" method="post">
	    {% for field in form %}
	        <div class="fieldWrapper">
	            {{ field.errors }}
	            {{ field.label_tag }} {{ field }}
	        </div>
	    {% endfor %}
	    {% csrf_token %}
	    <button class="btn btn-primary" type="submit">註冊</button>
	</form>
</div>
{% endblock content %}
```
