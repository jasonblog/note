import pandas as pd
import os
import json
from datetime import datetime


date = datetime.now().strftime("%Y%m/%d")


def getToday539Data():
    try:
        readData = pd.read_html('https://www.pilio.idv.tw/lto539/list.asp')[3]  # 取得網頁上的表格資訊
        get539NewDate = readData.loc[1][0][0:9]  # 取得第一列第六欄的字串前10個字元 也就是 日期
    except:
        readData = pd.read_html('https://www.pilio.idv.tw/lto539/list.asp')[2]  # 取得網頁上的表格資訊
        get539NewDate = readData.loc[1][0][0:9]  # 取得第一列第六欄的字串前10個字元 也就是 日期
    print("官網提供歷史紀錄↓↓↓\n%s" % readData)
    print("\n取得最新日期↓↓↓\n%s" % get539NewDate)
    if date == str(get539NewDate):  # 判斷今天日期是否與最新日期相同，不相同則回傳False
        readData.loc[0:1].to_json(os.getcwd() + '/new539Data.json')  # 第一至三列的數據存到json檔案
        return True
    else:
        return False

if getToday539Data() is True:
    print('\n取最新的539開獎號碼↓↓↓\n%s' % pd.read_json(os.getcwd() + '/new539Data.json'))  # 可以參考最新油價數據 Json 讀取最後儲存的油價數據
else:
    print("\n%s →→ 還未開獎" % date)


