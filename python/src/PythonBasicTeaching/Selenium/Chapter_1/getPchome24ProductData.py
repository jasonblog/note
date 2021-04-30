# -*- coding: utf-8 -*-
import json
import time
import os
import sys

from selenium import webdriver
from urllib.request import urlretrieve
import time

driver = webdriver.Firefox(executable_path = os.getcwd() + '/geckodriver')


def Buy_Main(buy_url):
    # 主要執行區塊

    buy_url_str = buy_url.split(',')
    for i in buy_url_str:
        driver.get(i)
        UrlBaseName = getUrlBaseName(buy_url=i)
        time.sleep(5)
        downloadImgStr(dirs_str = UrlBaseName, save_img_url = getTitlePhotoUrl(), save_img_name = getTitlePhotoName())
        downloadImgDict(dirs_str = UrlBaseName, save_img_dict = getContentsPhoto())
        data_str = dataIntegration(getTitleName(), getContentName(), getPriceTotal(), getContentsText(), getStandardEquipment(), getProductSpecification(), str(i))
        writeFile(dir_str = UrlBaseName, write_str = data_str)
    driver.quit()


def dataIntegration(*args):
    # 將取得的 標題 內容 金額 其他備註 整合起來
    data_str = '*' * 10 + '\n'
    data_list = []
    args = list(args)
    for arg in args:
        data_list.append(data_str + arg)
    all_data_str = '\n'.join(data_list)
    print(all_data_str)
    return all_data_str


def getUrlBaseName(buy_url):
    # 取得URL，Base Name
    getUrlName = os.path.basename(buy_url)
    getUrlName = getUrlName + '_' + str(time.strftime("%m%d%Y%I%M%S"))
    print(getUrlName)
    return getUrlName


def writeFile(dir_str, write_str):
    # 寫入到Url資料夾下的note.txt
    fp = open(os.getcwd() + "/" + dir_str + "/note.txt", "w")
    fp.write(write_str)
    fp.close()


def getContentsText():
    # 取得 本商品詳細介紹 文字
    ContentsTextList = []
    contents_data = driver.find_elements_by_css_selector("#IntroContainer dd")
    for content_data in contents_data:
        ContentsTextList.append(content_data.text)
    ContentsTextStr = '\n'.join(ContentsTextList)
    print(ContentsTextStr)
    return ContentsTextStr


def getContentsPhoto():
    # 取得 本商品詳細介紹 全部圖片
    ContentsImgList = []
    ContentsImgListName = []
    contents_data = driver.find_elements_by_css_selector("#IntroContainer dd img")
    for content_data in contents_data:
        ContentsImgList.append(content_data.get_attribute("src"))
        ContentsImgListName.append(os.path.basename(content_data.get_attribute("src")))
    ContentsDict = dict(zip(ContentsImgListName, ContentsImgList))
    print(ContentsDict)
    return ContentsDict


def getTitleName():
    # 取得網頁商品標題名稱
    title_name = driver.find_element_by_id("NickContainer").text
    print(title_name)
    return title_name


def getContentName():
    # 取得網頁商品大綱內容
    content_name = driver.find_element_by_id("SloganContainer").text
    print(content_name)
    return content_name


def getPriceTotal():
    # 取得網頁商品金額
    price = driver.find_element_by_id("PriceTotal").text
    print(price)
    return price


def getTitlePhotoUrl():
    # 取得網頁商品圖片連結
    title_photo_url = driver.find_element_by_css_selector("#ImgContainer img").get_attribute("src")
    print(title_photo_url)
    return title_photo_url


def getStandardEquipment():
    # 取得標準配件資訊
    standard_equipement = driver.find_element_by_css_selector("#EquipContainer").text
    print(standard_equipement)
    return standard_equipement


def getProductSpecification():
    # 取得 商品規格資訊
    product_specification = driver.find_element_by_css_selector("#StmtContainer").text
    print(product_specification)
    return product_specification


def getTitlePhotoName():
    # 取得網頁商品圖片的名稱
    title_photo_url = driver.find_element_by_css_selector("#ImgContainer img").get_attribute("src")
    title_photo_url_name = os.path.basename(title_photo_url)
    print(title_photo_url_name)
    return title_photo_url_name


def downloadImgStr(dirs_str, save_img_url, save_img_name):
    # 下載圖片到本地端，透過URL Base name 建立資料夾。
    os.makedirs(os.getcwd() + '/' + str(dirs_str), exist_ok = True)  # 建立目錄存放檔案
    print("建立資料夾: " + str(os.getcwd() + '/' + dirs_str))
    saveFilePath = os.getcwd() + '/' + str(dirs_str) + '/' + str(save_img_name)
    urlretrieve(save_img_url, saveFilePath)  # 儲存圖片
    print("下載檔案至: " + saveFilePath)
    print("下載完成。")


def downloadImgDict(dirs_str, save_img_dict):
    # 下載多個圖片到本地端，透過URL Base name 建立資料夾。

    os.makedirs(os.getcwd() + '/' + str(dirs_str), exist_ok = True)  # 建立目錄存放檔案
    for imgName, imgUrl in save_img_dict.items():
        saveFilePath = os.getcwd() + '/' + str(dirs_str) + '/' + str(imgName)
        urlretrieve(imgUrl, saveFilePath)  # 儲存圖片
        print("下載檔案至: " + saveFilePath)
    print("下載完成。")


def moveBottom():
    # 滑動到最下方
    js = "var q=document.documentElement.scrollTop=100000"
    driver.execute_script(js)
    print("滑動到頁面最下方")


if __name__ == '__main__':
    Buy_Main(buy_url = sys.argv[1])

