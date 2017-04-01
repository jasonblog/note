# Post 抓取高鐵

```py
import requests

payload ={
'StartStation':'fbd828d8-b1da-4b06-a3bd-680cdca4d2cd',
'EndStation':'9c5ac6ca-ec89-48f8-aab0-41b738cb1814',
'SearchDate':'2015/10/04',
'SearchTime':'19:00',
'SearchWay':'DepartureInMandarin'
}

res = requests.post('https://www.thsrc.com.tw/tw/TimeTable/SearchResult', verify=False , data = payload)
print res.text
```