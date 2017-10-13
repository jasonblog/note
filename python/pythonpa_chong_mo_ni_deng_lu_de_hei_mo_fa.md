# Python爬虫模拟登录的黑魔法


今天用 requests + BeautifulSoup 抓取 aliexpress 的时候， 在模拟登录时候出现了很多问题， 提交数据时会对密码等一些字段加密， 而且提交一大堆不知名的字段， 大概有二十多项。 看到那么多字段， 整个人就不好了， 作为一个懒人， 准备绕过这个坑。

大概思路是这样， 通过 selenium 打开浏览器， 模拟登录。 获取cookies ，并将cookies以文件的形式保存到本地。 当我们使用requests打开页面的时候就可以用本地的cookies。 因为 通过selenium打开浏览器的方式登陆没有那么多限制， 只需要模拟登录流程（ 输入帐号密码， 点击登录即可登陆）。 而且selenium可以模拟各种浏览器， 亦可以在命令行下实现浏览器功能。

###1: 通过selenium 登录

```py
from selenium import webdriver
from selenium.webdriver.common.keys import Keys

def login(username, password browser=None):
    browser.get("https://login.example.com/")

    pwd_btn = browser.find_element_by_name("password")
    act_btn = browser.find_element_by_name("loginId")
    submit_btn = browser.find_element_by_name("submit-btn")  

    act_but.send_keys(username)
    pwd_btn.send_keys(password)
    submint_btn.send_keys(Keys.ENTER)

    return browser
```

###2: 保存Cookie

通过seleum 模拟登陆， 然后将cookie打包保存到本地

将获取的cookies 存放在变量cookies中

```py
import requests

request = requests.Session()
headers = {
        "User-Agent":
            "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 "
            "(KHTML, like Gecko) Chrome/44.0.2403.157 Safari/537.36"}
request.headers.update(headers)
cookies = browser.get_cookies()
```

###cookie保存在本地

```py
# save the current cookies as a python object using pickle 

import selenium.webdriver 
import pickle

pickle.dump( cookies , open("cookies.ini","wb"))
```

###我们可以用同样的方法将cookies重新载入

```py
# add them back:
browser = selenium.webdriver.Firefox()
browser.get("http://www.example.com")
cookies = pickle.load(open("cookies.pkl", "rb"))
for cookie in cookies: 
     browser.add_cookie(cookie)

```

###3: 使用本地的cookie

```py
import pickleimport selenium.webdriver 
import pickle
import requests

cookies = pickle.load(open("cookies.pkl", "rb"))
request = requests.Session()
headers = {
        "User-Agent":
            "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 "
            "(KHTML, like Gecko) Chrome/44.0.2403.157 Safari/537.36"}
request.headers.update(headers)


for cookie in cookies:
    request.cookies.set(cookie['name'], cookie['value'])

# 加载cookies后登录网站
request.get("www.example.com")
```

###完整的例子

```py
import pickle
import requests
from selenium import webdriver
from selenium.webdriver.common.keys import Keys

def setup():
    browser = webdriver.Firefox()
    return browser

def login(username, password browser=None):
    browser.get("https://login.example.com/")

    pwd_btn = browser.find_element_by_name("password")
    act_btn = browser.find_element_by_name("loginId")
    submit_btn = browser.find_element_by_name("submit-btn")  

    act_but.send_keys(username)
    pwd_btn.send_keys(password)
    submint_btn.send_keys(Keys.ENTER)

    return browser

def set_sessions(browser):
    request = requests.Session()
    headers = {
        "User-Agent":
            "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 "
            "(KHTML, like Gecko) Chrome/44.0.2403.157 Safari/537.36"
    }
    request.headers.update(headers)
    cookies = browser.get_cookies()
    for cookie in cookies:
        request.cookies.set(cookie['name'], cookie['value'])

    return request


if __name__ == "__main__":
    browser = login("textusername", "tespassword", setup())
    rq = set_sessions(browser)
    re.get("http://www.example.com")

```
###友情提示

由于selenium 下的phantomJS在使用过程中出现了不知原因的各种坑。 所以在Python中我们可以用下面的方法，在命令行打开Chrome、FireFox。

```py
from pyvirtualdisplay import Display
from selenium import webdriver

display = Display(visible=0, size=(1024, 768))
display.start()
chromedriver = "/root/project/driver/chromedriver"
os.environ["webdriver.chrome.driver"] = chromedriver
browser = webdriver.Chrome(chromedriver)
```