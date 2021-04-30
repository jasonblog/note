# step1. import package
import requests
import pandas as pd
import os
from io import StringIO
from datetime import datetime
import json

date = datetime.now().strftime("%Y%m%d")
print('查詢日期: %s' % date)

r = requests.post('http://www.twse.com.tw/exchangeReport/MI_INDEX?response=csv&date=' + date + '&type=ALL')

print(r.text)
# # 篩選出個股盤後資訊
# str_list = []
# for i in r.text.split('\n'):
#     if len(i.split('",')) == 17 and i[0] != '=':
#         i = i.strip(",\r\n")
#         str_list.append(i)
#
# # 印出選股資訊 轉成json
# df = pd.read_csv(StringIO("\n".join(str_list)))
# pd.set_option('display.max_rows', None)
#
# with open(os.getcwd() + '/' + date + ".json", 'w') as json_file:
#     json.dump(df.to_dict(orient='index'), json_file)
#
# print("json 檔案儲存至: %s" % os.getcwd() + '/' + date + ".json")
#

