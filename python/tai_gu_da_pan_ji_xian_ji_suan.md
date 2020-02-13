# 台股大盤季線計算


```py
import pandas as pd
import requests,json
from time import sleep
from datetime import datetime, timedelta, date
from collections import OrderedDict

#將數字字串裡的千分符','用''取代，例如123,456轉成123456
def num_comma_clear(arg):
    return str(arg.replace(",",""))

now_time = datetime.now()
change_time = now_time + timedelta(days=-90)
change_time_format = change_time.strftime('%Y-%m-%d')
print(change_time_format)

dates = [change_time_format, str(date.today().strftime("%Y-%m-%d"))]
start, end = [datetime.strptime(_, "%Y-%m-%d") for _ in dates]
month_list=OrderedDict(((start + timedelta(_)).strftime(r"%Y%m01"), None) for _ in range((end - start).days)).keys()

with open('twse_100.csv','w',encoding='utf-8') as f:
    f.writelines('日期,開盤指數,最高指數,最低指數,收盤指數\n')
f.close()
#記錄下載失敗的url到 twii_2a_error.log
with open('twse_100_error.log','w',encoding='utf-8') as f:
    pass
f.close()

for i in month_list:
    url="https://www.twse.com.tw/indicesReport/MI_5MINS_HIST?response=json&date=%s"%i
    try:
        json_source=requests.get(url).text
        json_data = json.loads(json_source)
        for j in json_data['data']:
            with open('twse_100.csv','a',encoding='utf-8') as f:
                f.writelines("%s,%s,%s,%s,%s\n"%(str(j[0]).replace(" ",""),num_comma_clear(j[1]),num_comma_clear(j[2]),num_comma_clear(j[3]),num_comma_clear(j[4])))
        print('下載年份月份: %s 完成!'%i[:-2])
    except:
        with open('twse_100_error.log','a',encoding='utf-8') as f:
            f.writelines("%s"%url)
        f.close()
        print('下載年份月份: %s 失敗! 已記錄至error.log'%i[:-2])
    finally:
        sleep(5)

stock = pd.read_csv('./twse_100.csv')
print(stock['收盤指數'].mean())
```

