# 個股年成交資訊

```py
# coding=utf-8
import requests
payload ={
'download':'csv',
'CO_ID':'2103'
}
res = requests.post('http://www.twse.com.tw/ch/trading/exchange/FMNPTK/FMNPTKMAIN.php', verify=False , data = payload)
print (res.content).decode("big5").encode("utf-8")

```

