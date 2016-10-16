# 如何抓取廉價航空的機票價格 – 以酷航

- http://www.flyscoot.com/zhtw//



```py
import requests
from bs4 import BeautifulSoup as bs

payload = {
'availabilitySearch.SearchInfo.SearchStations[0].DepartureStationCode':'TPE',
'availabilitySearch.SearchInfo.SearchStations[0].ArrivalStationCode':'NRT',
'availabilitySearch.SearchInfo.SearchStations[0].DepartureDate':'8/17/2016',
'availabilitySearch.SearchInfo.SearchStations[1].DepartureStationCode':'',
'availabilitySearch.SearchInfo.SearchStations[1].ArrivalStationCode':'',
'availabilitySearch.SearchInfo.SearchStations[1].DepartureDate':'9/13/2016',
'availabilitySearch.SearchInfo.Direction':'Return',
'availabilitySearch.SearchInfo.AdultCount':'1',
'availabilitySearch.SearchInfo.ChildrenCount':'0',
'availabilitySearch.SearchInfo.InfantCount':'0',
'availabilitySearch.SearchInfo.PromoCode':'',
}

rs = requests.session()
res = rs.post("http://makeabooking.flyscoot.com/?culture=zh-TW", data=payload, verify=False)
res2 = rs.get("http://makeabooking.flyscoot.com/Flight/Select", verify=False)

#print res2.text

soup = bs(res2.text, "lxml")
for li in soup.select('.departure li'):
    if len(li.select('.flightdate')) > 0:
        print li.select('.flightdate')[0].text, li.select('.flight_price')[0].text

```

