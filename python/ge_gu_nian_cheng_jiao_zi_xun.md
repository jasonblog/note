# 個股年成交資訊

```py
# coding=utf-8
import requests

payload = {
'download':'csv',
'CO_ID':'2103'
}

res = requests.post('http://www.twse.com.tw/ch/trading/exchange/FMNPTK/FMNPTKMAIN.php', verify=False , data = payload)

#print type(res.content)
#print type(res.content.decode("big5"))
#print type(res.content.decode("big5").encode("utf-8"))

print (res.content).decode("big5").encode("utf-8")

with open("aa.csv", "w+") as f:
    f.write(str(res.content).decode("big5").encode("utf-8"))

```

