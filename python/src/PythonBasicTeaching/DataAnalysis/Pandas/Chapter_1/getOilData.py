import pandas as pd
import os
import json
from datetime import datetime


date = datetime.now().strftime("%Y/%m/%d")


def getTodayOilData():
    try:
        readData = pd.read_html('https://www2.moeaboe.gov.tw/oil102/oil2017/A01/A0108/tablesprices.asp')[1]  # 取得網頁上的表格資訊
        getOilNewDate = readData.loc[1][6][0:10]  # 取得第一列第六欄的字串前10個字元 也就是 日期
    except:
        readData = pd.read_html('https://www2.moeaboe.gov.tw/oil102/oil2017/A01/A0108/tablesprices.asp')[0]  # 取得網頁上的表格資訊
        getOilNewDate = readData.loc[1][6][0:10]  # 取得第一列第六欄的字串前10個字元 也就是 日期

    print("官網提供歷史紀錄↓↓↓\n%s" % readData)
    print("\n取得最新日期↓↓↓\n%s" % getOilNewDate)
    if date == str(getOilNewDate):  # 判斷今天日期是否與最新日期相同，不相同則回傳False
        readData.loc[0:2].to_json(os.getcwd() + '/newOilData.json')  # 第一至三列的數據存到json檔案
        return True
    else:
        return False


if getTodayOilData() is True:
    print('\n取最新的數據↓↓↓\n%s' % pd.read_json(os.getcwd() + '/newOilData.json'))  # 可以參考最新油價數據 Json 讀取最後儲存的油價數據
else:
    print("\n%s →→ 油價沒調整" % date)


