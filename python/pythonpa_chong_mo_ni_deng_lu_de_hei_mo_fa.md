# Python爬蟲模擬登錄的黑魔法


今天用 requests + BeautifulSoup 抓取 aliexpress 的時候， 在模擬登錄時候出現了很多問題， 提交數據時會對密碼等一些字段加密， 而且提交一大堆不知名的字段， 大概有二十多項。 看到那麼多字段， 整個人就不好了， 作為一個懶人， 準備繞過這個坑。

大概思路是這樣， 通過 selenium 打開瀏覽器， 模擬登錄。 獲取cookies ，並將cookies以文件的形式保存到本地。 當我們使用requests打開頁面的時候就可以用本地的cookies。 因為 通過selenium打開瀏覽器的方式登陸沒有那麼多限制， 只需要模擬登錄流程（ 輸入帳號密碼， 點擊登錄即可登陸）。 而且selenium可以模擬各種瀏覽器， 亦可以在命令行下實現瀏覽器功能。

###1: 通過selenium 登錄

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

通過seleum 模擬登陸， 然後將cookie打包保存到本地

將獲取的cookies 存放在變量cookies中

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

###我們可以用同樣的方法將cookies重新載入

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

# 加載cookies後登錄網站
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

由於selenium 下的phantomJS在使用過程中出現了不知原因的各種坑。 所以在Python中我們可以用下面的方法，在命令行打開Chrome、FireFox。

```py
from pyvirtualdisplay import Display
from selenium import webdriver

display = Display(visible=0, size=(1024, 768))
display.start()
chromedriver = "/root/project/driver/chromedriver"
os.environ["webdriver.chrome.driver"] = chromedriver
browser = webdriver.Chrome(chromedriver)
```