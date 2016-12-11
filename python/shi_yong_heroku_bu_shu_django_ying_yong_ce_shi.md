# 使用heroku部署django應用測試


### 1 如果沒有heroku帳號，先註冊
```
https://api.heroku.com/signup/devcenter
```

### 2 如果沒有安裝Heroku Toolbelt，先安裝。（https://toolbelt.heroku.com）
```
wget -qO- https://toolbelt.heroku.com/install-ubuntu.sh | sh
```

### 3 建立本地端軟體環境
```
virtualenv venv
source venv/bin/activate
```

### 4 登錄
```
heroku login
```

### 5 創建 Django app(1.6)
```
$ mkdir hellodjango && cd hellodjango
$ workon hellodjango
$ pip install django-toolbelt
$ django-admin.py startproject hellodjango .
# 在專案根目錄新建一個名為Procfile的檔案，內容為
web: gunicorn hellodjango.wsgi
# Local運行下app
$ python manage.py runserver 0.0.0.0:8000
# 到處依賴
$ pip freeze > requirements.txt
```

### 6 靜態文件放在project目錄下的static中 修改settings.py，在末尾加上：
```
# Parse database configuration from $DATABASE_URL
import dj_database_url
DATABASES['default'] =  dj_database_url.config()

# Honor the 'X-Forwarded-Proto' header for request.is_secure()
SECURE_PROXY_SSL_HEADER = ('HTTP_X_FORWARDED_PROTO', 'https')

# Allow all host headers
ALLOWED_HOSTS = ['*']

# Static asset configuration
import os
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
STATIC_ROOT = 'staticfiles'
STATIC_URL = '/static/'

STATICFILES_DIRS = (
    os.path.join(BASE_DIR, 'static'),
)
```

### 7 修改wsgi.py，在末尾添加：
```
from django.core.wsgi import get_wsgi_application
from dj_static import Cling

application = Cling(get_wsgi_application())
```

### 8 將項目放到git
```
git init
git add .
git commit -m 'first commit'
```

### 9 部署到heroku，在項目目錄運行
```
heroku create app_name
git push heroku master
```
```
查看部署好的app：heroku open

查看log：heroku logs

查看進程：heroku ps

運行shell命令：heroku run python manage.py syncdb

這是我剛測試部署的django網站，http://ashin.herokuapp.com/

克隆heroku上的代碼到本地：git clone git@heroku.com:APP_NAME.git

提示找不到repo的話：heroku git:remote -a APP_NAME

```

