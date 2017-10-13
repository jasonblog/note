# ptt  using  selenium


```py
# -*- coding: utf-8 -*

from selenium import webdriver
from bs4 import BeautifulSoup
driver = webdriver.PhantomJS()
driver.get("https://www.ptt.cc/bbs/Gamesale/index.html")
soup = BeautifulSoup(driver.page_source, "lxml")
for article in soup.select('.r-list-container .r-ent .title a'):
    title = str(article.string)
    print(title)


```