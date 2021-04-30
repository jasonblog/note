from selenium import webdriver
import os

driver = webdriver.Firefox(executable_path=os.getcwd() + '/geckodriver')  # 開啟firefox

driver.get("https://www.google.com.tw/")  # 前往Google

driver.close()  # 將瀏覽器關閉

