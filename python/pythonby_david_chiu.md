# Python_by_David_Chiu

- ptt Gossiping

```py
# ptt extraction
import requests
from bs4 import BeautifulSoup
rs = requests.session()
payload = {
'from':'/bbs/Gossiping/index.html',
'yes':'yes'
}
res1 = rs.post("https://www.ptt.cc/ask/over18", data=payload, verify=False)
for i in range(7649, 7600, -1):
    res = rs.get("https://www.ptt.cc/bbs/Gossiping/index%d.html"%i, verify=False)
    soup = BeautifulSoup(res.text)
    tr = soup.select(".r-ent")
    for rec in  tr:
       # print rec.select('.title')[0].text.encode('utf-8').strip()
        if "江" in rec.select('.title')[0].text.encode('utf-8').strip():
            print  rec.select('.title')[0].text.strip()
```

- highspeed_rail

```py
import requests
from bs4 import BeautifulSoup
payload ={
'StartStation':'977abb69-413a-4ccf-a109-0272c24fd490',
'EndStation':'fbd828d8-b1da-4b06-a3bd-680cdca4d2cd',
'SearchDate':'2015/01/07',
'SearchTime':'10:00',
'SearchWay':'DepartureInMandarin'          
}
res = requests.post("http://www.thsrc.com.tw/tw/TimeTable/SearchResult", data=payload)
soup = BeautifulSoup(res.text)
timetable = soup.select(".time_check_result")
timetable[0].select('tr')

for tr in timetable[0].select('tr'):
    print "%s\t%s\t%s"%(tr.select(".column1")[0].text,tr.select(".column3")[0].text,tr.select(".column4")[0].text)

```

- FB_GRAPH_API

```py
import requests
import json
access_token = '<acces token>'
res = requests.get("https://graph.facebook.com/v2.2/me/friends?access_token=%s"%(access_token))
js = json.loads(res.text)
for i in js['data']:
    print i['name'], i['id']
```


```py
# -*- coding: utf-8 -*- 
from selenium import selenium 
from selenium import webdriver
from selenium.common.exceptions import NoSuchElementException
from selenium.webdriver.common.keys import Keys
import time

browser = webdriver.Firefox() 
#browser = webdriver.Ie('IEDriverServer.exe') 

browser.get('http://jirs.judicial.gov.tw/FJUD/FJUDQRY01_1.aspx') 
time.sleep(3) 
elem = browser.find_element_by_name("dy1") 
elem.send_keys(u'103' + Keys.RETURN) 
print browser.find_element_by_tag_name("body").text


browser.close()
```

```py
import requests
res = requests.get("http://140.115.236.11/query_tea.asp?STR=ZA101&Query=%ACd%B8%DF&MM2=1&YY2=2015&item2=%AFZ%AF%C5")
res.encoding = "big5"
print res.text
```

```py
import requests
header = {'user-agent':'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36'}
res = requests.get("https://www.google.com.tw/search?tbs=sbi%3AAMhZZisDdaia0AXISbyJd4X5hHQCRAQCgZMV-EbUWeSAAu1pRFVlaPE8OFg9ABtUKsyL1Mv-FEH0V6Au_1jUrygpKCIufrYROqHRkaFeELfwD40fp03kp2bSKJFZL9T_1VR_1yyXV_1Rl-Eq9_1a9JfuY-8QC_1fPWOISfFyVfSVB4NR8F63F-Gh4shJyYrFN5S144Cst9qQtmonNcxMKitRyCVMhzViFlUrUi8rr-3ZhJdjKcHS-GcoxbFoy6ima-ngIHoIjlQh7B462BjDdOTppgwwy1KK7-SqY11ai-IyapTJec7JkFS_182sfULocSDtL3TZiwDxLiUP1GZws9fTtgQp1wmzRS78JcoEups3pgh_1CAIqIj9cb22eABqchmgdr2cxIHVxJzhqh491nHs1_1qbLY8Hb9r03JxvErYf8r-Ju-j7LjdwHhpqwA1U2HB6tHkjmnz5qvw1GciQiLHlierin3aPAxqw3w4W959fOlot4hS5YjceWFREvxtELeSQI-UgPfGkgbCkCZrlmqXAwLh9gEahKdkwecjCWdEliC9AxL_1j4bkerV3itA69S8_1Uw4k7CWPLV-hNfzhNQkXPGSnUrhMpVXJGrteAfyxUtSqhZ5EujWkkzY8PWrcV8BwFfeDth_1gBNbT5kCMyyLLHErK2KWPgls8-mg7-RqxcyhP5UV79qA9RMk4bkVjz_1vnoNChzJv_15gZqWTQeYqUzPVEKLi7BgyMCLNCXhw3jAUD3oeJ0aYlffOPM0iCO1CxbMXr-wqG_1QdqzC2ed_19I5Juvz0SLWgqGsDdXm-oN3aEb1-htaXwMLhepm3kvXrTeqCwNhfeWuvsXz2lW14qd8dtTI4hr8WMAZEPNWsac3v9VbyRegzs8JB52GMo-jcVSUDKefHcrrBsg59LHIjIM7s4u_1UUbuSXt7HFPOI2o044fmWsk18FC_1ATbKdjXHyKxnnvedpYa9m_1UJZlzAvweOXaxO3RTFskQL5vS4yVy3IpuwRGdww7KpPfC0ZDIiGiVs2e5xBH4QaJMsaCHcDQeAamAqH8kftefPszuREL6RzRv189fEkZTIg0o-RLc2rJfjZmqQWbmGEB1h_1NBISzCLOewafC5zyVh7Vd7mdFfBj89_1VVmitlqGILb_12iCOH8oMmw9_1pO2etB9Z5fKbK2ALa0R7Q2MO4kpZZamgKoZNVsIswG9_1sSkF_1v--hk9_1wZs2XDUwn7vfK1pniBx9e5UaiTTFhJZ4IX-Xo8udOLdsfTYa7b77Ljo09wBbGCqML5dH9oLsnPaywDDoe1cv9FmBgQsv23gfGEvn6HuUJwqVOG6y8PPcqRkP8j2rSn2MbgTyd4Oq0M0tic_1d9ds8QkwkTW9tSl1c0cK2D55W17kOMZnoQAr5rB5N_1zA6OgerTGevOvydt8kx7_1elGEzUeL1cJhU8nlWf_1Rf8qFBeoNqS71hKomupwVHfWa-Y7ef4UpaZHP8vN2EVBTSpskiPQb59yx3nWXhzojbCueMWs8A&btnG=%E4%BB%A5%E5%9C%96%E6%90%9C%E5%B0%8B", headers = header)
print res.text
```

```py
import requests
header = {'user-agent':'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36'}
res = requests.get("https://www.google.com.tw/search?tbs=sbi%3AAMhZZisDdaia0AXISbyJd4X5hHQCRAQCgZMV-EbUWeSAAu1pRFVlaPE8OFg9ABtUKsyL1Mv-FEH0V6Au_1jUrygpKCIufrYROqHRkaFeELfwD40fp03kp2bSKJFZL9T_1VR_1yyXV_1Rl-Eq9_1a9JfuY-8QC_1fPWOISfFyVfSVB4NR8F63F-Gh4shJyYrFN5S144Cst9qQtmonNcxMKitRyCVMhzViFlUrUi8rr-3ZhJdjKcHS-GcoxbFoy6ima-ngIHoIjlQh7B462BjDdOTppgwwy1KK7-SqY11ai-IyapTJec7JkFS_182sfULocSDtL3TZiwDxLiUP1GZws9fTtgQp1wmzRS78JcoEups3pgh_1CAIqIj9cb22eABqchmgdr2cxIHVxJzhqh491nHs1_1qbLY8Hb9r03JxvErYf8r-Ju-j7LjdwHhpqwA1U2HB6tHkjmnz5qvw1GciQiLHlierin3aPAxqw3w4W959fOlot4hS5YjceWFREvxtELeSQI-UgPfGkgbCkCZrlmqXAwLh9gEahKdkwecjCWdEliC9AxL_1j4bkerV3itA69S8_1Uw4k7CWPLV-hNfzhNQkXPGSnUrhMpVXJGrteAfyxUtSqhZ5EujWkkzY8PWrcV8BwFfeDth_1gBNbT5kCMyyLLHErK2KWPgls8-mg7-RqxcyhP5UV79qA9RMk4bkVjz_1vnoNChzJv_15gZqWTQeYqUzPVEKLi7BgyMCLNCXhw3jAUD3oeJ0aYlffOPM0iCO1CxbMXr-wqG_1QdqzC2ed_19I5Juvz0SLWgqGsDdXm-oN3aEb1-htaXwMLhepm3kvXrTeqCwNhfeWuvsXz2lW14qd8dtTI4hr8WMAZEPNWsac3v9VbyRegzs8JB52GMo-jcVSUDKefHcrrBsg59LHIjIM7s4u_1UUbuSXt7HFPOI2o044fmWsk18FC_1ATbKdjXHyKxnnvedpYa9m_1UJZlzAvweOXaxO3RTFskQL5vS4yVy3IpuwRGdww7KpPfC0ZDIiGiVs2e5xBH4QaJMsaCHcDQeAamAqH8kftefPszuREL6RzRv189fEkZTIg0o-RLc2rJfjZmqQWbmGEB1h_1NBISzCLOewafC5zyVh7Vd7mdFfBj89_1VVmitlqGILb_12iCOH8oMmw9_1pO2etB9Z5fKbK2ALa0R7Q2MO4kpZZamgKoZNVsIswG9_1sSkF_1v--hk9_1wZs2XDUwn7vfK1pniBx9e5UaiTTFhJZ4IX-Xo8udOLdsfTYa7b77Ljo09wBbGCqML5dH9oLsnPaywDDoe1cv9FmBgQsv23gfGEvn6HuUJwqVOG6y8PPcqRkP8j2rSn2MbgTyd4Oq0M0tic_1d9ds8QkwkTW9tSl1c0cK2D55W17kOMZnoQAr5rB5N_1zA6OgerTGevOvydt8kx7_1elGEzUeL1cJhU8nlWf_1Rf8qFBeoNqS71hKomupwVHfWa-Y7ef4UpaZHP8vN2EVBTSpskiPQb59yx3nWXhzojbCueMWs8A&btnG=%E4%BB%A5%E5%9C%96%E6%90%9C%E5%B0%8B", headers = header)
print res.text
```

```py
import requests
import json
res = requests.get("http://www.gretai.org.tw/web/stock/aftertrading/daily_trading_info/st43_result.php?l=zh-tw&d=104%2F01&stkno=3290&_=1420965808599")
j = json.loads(res.text)
print j['stkName']
```

```py
import requests
import json
url ="http://www.gretai.org.tw/web/stock/aftertrading/daily_trading_info/st43_result.php?l=zh-tw&d=%s&stkno=3290&_=1421136081459"
dt = "104/1"
from datetime import timedelta
from dateutil.relativedelta import relativedelta

getyear =  dt.split('/',1) 
bctime =  str(int(getyear[0]) + 1911) +"/"+ getyear[1]
for i in range(1,10):
    ts =  (datetime.strptime(bctime, "%Y/%m") - relativedelta(months=i)).strftime("%Y/%m")
    ts2 = ts.split('/',1)
    ts3 = str(int(ts2[0]) - 1911) + '/' + ts2 [1]
    res = requests.get(url %(ts3))
    j = json.loads(res.text)
    for price in j['aaData']:
        print price[0]
```

```py
#! coding = UTF-8 
__author__ = 'john.chen' 
import requests 
from bs4 import BeautifulSoup 
index = 1 # 排行名次 
for i in range(1, 5): # 共5頁 
    res = requests.get("https://tw.news.yahoo.com/sentiment/informative/%d.html"%i, verify=False) 
    soup = BeautifulSoup(res.text) 
    for txt in soup.select("#mediasentimentlisttemp .story"): 
        print index, 
        index += 1 
        print txt.text.strip()
```

```py
#-*- coding:utf-8 -*-
import requests,time
from bs4 import BeautifulSoup
for i in range(88,89):   #跑88~103年
    for j in range(1,2):  #跑1-12月份
        #header = {'user-agent':'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36'}
        payload={
            'myear':'%d'%i,
            'mmon':'%s'%str(j +100)[1:]   #%d integer 整數
        }
        user_post=requests.post('http://www.twse.com.tw/ch/trading/indices/MI_5MINS_HIST/MI_5MINS_HIST.php',data=payload)
        user_post.encoding = "big5"
        #print user_post.text.encode('utf-8')
        soup=BeautifulSoup(user_post.text.encode('utf-8'))
        for td in soup.select(".board_trad"):    #從文件裡面取得所有的連結，board_trad是table class
            print td.text.encode('utf-8') 
            #time.sleep(5)
```


```py
import requests
from bs4 import BeautifulSoup
 
for i in range(1,13):
    if i <10:
        month = "0" + str(i)
    else:
        month = i
    for j in range(1,32):
        if j <10:
            day = "0" + str(j)
        else:
            day = j
 
        res = requests.get("http://www.twse.com.tw/ch/trading/fund/BFI82U/BFI82U.php?report1=day&input_date=104%2F01%2F29&mSubmit=%ACd%B8%DF&yr=2015&w_date=20150126&m_date=20150101")
        res.encoding = "big5"
        soup = BeautifulSoup(res.text)
        for td in soup.select(".board_trad tr"):
            print td.text.strip()
```

```py
# encode=utf-8
 
import requests
from bs4 import BeautifulSoup
 
res = requests.get("http://www.accupass.com/search/r/1/5/0/0/1/0", verify = False)
soup = BeautifulSoup(res.text)
 
for event in soup.select(".APCSS_linkBox-event"):
    print ' '.join(event.text.split())
```


```py
import requests
import json
res  =  requests.get("https://graph.facebook.com/v2.2/divajody/posts?access_token=<access_token>&format=json&method=get&pretty=0&suppress_http_code=1")
js = json.loads(res.text)
for mes in  js['data']:
    if 'message' in mes:
        print mes['message']
```

```py
import requests
from bs4 import BeautifulSoup
res = requests.get("http://www.mobile01.com/category.php?id=6")
soup = BeautifulSoup(res.text)
grp = soup.select(".item .group")
for item in grp:
    print "http://www.mobile01.com/"+ item['href'], item.select('img')[0]['src'], item.select('h3')[0].text, item.select('p')[0].text
```

```py
! coding=UTF-8
import requests,time
from bs4 import BeautifulSoup
#group = ['3474','4938','3481','2330','2303','2882','2357','1303','2883',\
#         '1301','2002','2311','2317','1402','2892','2880','2801','1216',\
#         '1101','1102','2382','2308','1326','2886','2891','2325','2105',\
#         '2395','2408','2412','2409','2207','2301','9904','2912','2354',\
#         '2474','3045','2454','2881','2887','4904','2885','3008','2498',\
#         '2884','2890','6505','5880','2227','0050']
group = ['3474']
for stock in group:   
    bid_detail=open(stock+".txt",'w')
    for i in range(2014,2015): 
        for j in range(1,13): 
            year = i
            month=j
            try:
                url = "http://www.twse.com.tw/ch/trading/exchange/STOCK_DAY/genpage/Report{}{}/{}{}_F3_1_8_{}.php?STK_NO={}&myear={}&mmon={}#"
                res = requests.get(url.format(year,month,year,month,stock,stock,year,month))     
            except:
                pass
            # print res.encoding 
            soup=BeautifulSoup(res.text.encode('ISO-8859-1'))
            for tr in soup.select(".board_trad tr")[2:-1]: 
                for td in tr.select('td'):
                    print td.text,
                    bid_detail.write(td.text.encode('utf-8'))
                print 
                bid_detail.write("\n")
bid_detail.close()
time.sleep(3)
```

```py
f = open("0050_bid_detail.txt", "r")
for line in f.readlines():
    element = line.strip().split()
    day = element[0]
    print day
f.close()
```

```py
import requests
import re
from bs4 import BeautifulSoup
import json
payload = {
'Cookie':'_ts_id=3C0C3D0B3E09300333; _gat=1; _ga=GA1.3.1930341740.1424415555',
'Host':'mybid.ruten.com.tw',
'User-Agent':'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.89 Safari/537.36'
}
res = requests.get('http://mybid.ruten.com.tw/credit/point?bestmask&sell', headers=payload)
res.encoding = 'big5'
li =  re.search('var f_list=(.*);',res.text)

jele = json.loads(li.group(1))
for ele in  jele['OrderList']:
        print ele['date'], ele['name'], ele['money'], 'http://goods.ruten.com.tw/item/show?' + ele['no']

```

```py
import requests
from bs4 import BeautifulSoup
import json
res = requests.get("http://graphics8.nytimes.com/packages/js/nytint/projects/tax_incentives/states/grants/AL.json?true=grants")

jele = json.loads(res.text.split("grants(")[1].encode('utf-8').strip(")"))
for ele in  jele:
    s = BeautifulSoup(ele[2])
    print ele[0], ele[1], s.text, ele[3]
```

```py
import requests
import re
from bs4 import BeautifulSoup
import json
res = requests.get('http://maps.googleapis.com/maps/api/geocode/json?address=%E5%8F%B0%E5%8C%97%E5%B8%82%E5%A4%A7%E5%AE%89%E5%8D%80%E5%AE%89%E5%92%8C%E8%B7%AF23%E5%B7%B711%E8%99%9F1%E6%A8%93&sensor=true_or_false')
jele = json.loads(res.text)
for address in jele['results']:
    print address['formatted_address'], address['geometry']['location']
```

```py
import requests
from bs4 import BeautifulSoup

domain = 'http://welcome2japan.tw/attractions/rest/'

dic = {}
res = requests.get("http://welcome2japan.tw/attractions/rest/")
soup = BeautifulSoup(res.text)
for url in soup.select('.green a'):
    if not (domain + url['href']) in dic:  
            dic[domain + url['href']] = domain + url['href']
for item in dic:
    print item
```

```py
#從links.txt的每一個link去爬卻爬不出冬西

links = open('links.txt', 'r')
for row in links.readlines():
    print row.strip()
    page = requests.get(row.strip())
    soup = BeautifulSoup(page.text.encode('utf8'))
    x = soup.select('center table table table tr')
    #print x
    for i in x:
        if len(i.select('td'))>0:
            try:
                print i.select('font')[0].text.strip()+','+i.select('font')[1].text.strip()+','+i.select('font')[2].text.strip()+','+i.select('font')[3].text.strip()
            except:
                pass
```

```py

import requests
from bs4 import BeautifulSoup
for page in range(1,2):
    res = requests.get('http://www.gotokyo.org/search/tw/result/list?q=&cx=015974231524706679776%3Amw9yltsgebg&ie=UTF-8&cof=FORID%3A9&genre=&accessibility=&use_acc_js=1&area_cd=&city_cd=&keyword=&mode=&pno_list={0}&pno_photo=&status=1&category=001&language=tw'.format(page))
    res.encoding = 'utf-8'
    soup = BeautifulSoup(res.text)
    for ele in soup.select('.box_result_list'):
        print ele.select('a')[0].text
```

    
    

```py
from bs4 import BeautifulSoup
text = '''<html><option value="A">臺北市</option>
<option value="C">基隆市</option>
<option value="F">新北市</option></html>'''

soup = BeautifulSoup(text)
for val in soup.select('option'):
    print val['value']
```

```py
import requests
from bs4 import BeautifulSoup
res = requests.get('http://allrecipes.com/recipes/appetizers-and-snacks/main.aspx')
soup = BeautifulSoup(res.text)
print soup.select('.ingredientSrchBtn')
```

```py
import re
f = open('xaa', 'r')
f2 = open('xaa.out','w')
for line in f.readlines():
        ele =  line.strip()
        m1 = re.match('.*act=(.*?);.*', ele)
        act = m1.group(1) if m1 else None
        m2 = re.match('.*uid=(.*?);.*', ele)
        uid = m2.group(1) if m2 else None
        m3 = re.match('.*erUid=(.*?);.*', ele)
        erUid = m3.group(1) if m3 else None
        m4 = re.match('.*pid=(.*?);.*', ele)
        pid = m4.group(1) if m4 else None
        m5 = re.match('.*cat=(.*?);.*', ele)
        cat = m5.group(1) if m5 else None
        f2.write("%s\t%s\t%s\t%s\t%s\n"%(act, uid, erUid, pid, cat))
f2.close()
f.close()
```


```py
import re, sys,json
f = open('out/MergeParsed.out','r')
dic = {}
for line in f.readlines():
        ele =  line.strip()
        info = ele.split('\t')
        if info[1] == 'view':
                if info[2] not in dic:
                        dic[info[2]] = {'uid': info[2], 'view':[info[4]]}
                else:
                        dic[info[2]]['view'].append(info[4])
        elif info[1] == 'order':
                if info[2] in dic:
                        dic[info[2]]['order'] = info[6]
                        print json.dumps(dic[info[2]])
                        del dic[info[2]]
f.close()
```

```py
import re, sys
n = sys.argv[1]
f = open(n, 'r')
f2 = open('%s.out'%(n),'w')
for line in f.readlines():
        ele =  line.strip()
        m0 = re.match(".*- - \[(.*?)\].*", ele)
        dt = m0.group(1) if m0 else None
        m1 = re.match('.*act=(.*?);.*', ele)
        act = m1.group(1) if m1 else None
        m2 = re.match('.*uid=(.*?);.*', ele)
        uid = m2.group(1) if m2 else None
        m3 = re.match('.*erUid=(.*?);.*', ele)
        erUid = m3.group(1) if m3 else None
        m4 = re.match('.*pid=(.*?);.*', ele)
        pid = m4.group(1) if m4 else None
        m5 = re.match('.*cat=(.*?);.*', ele)
        cat = m5.group(1) if m5 else None
        m6 = re.match('.*plist=(.*?);.*', ele)
        plist = m6.group(1) if m6 else None
        if uid != '':
                f2.write("%s\t%s\t%s\t%s\t%s\t%s\t%s\n"%(dt,act, uid, erUid, pid, cat, plist))
f2.close()
f.close()
```


```py
import requests
from bs4 import BeautifulSoup
res = requests.get('https://www.ptt.cc/bbs/Food/M.1431510360.A.FEB.html', verify=False)
soup = BeautifulSoup(res.text)
#print soup
print soup.select('#main-container')[0].text
```

```py
import requests
from bs4 import BeautifulSoup
res = requests.get('https://www.ptt.cc/bbs/Food/M.1431510360.A.FEB.html', verify=False)
soup = BeautifulSoup(res.text)
#print soup
print soup.select('#main-content')[0].contents[4]
```

```py
import requests
from bs4 import BeautifulSoup
res = requests.get('https://www.ptt.cc/bbs/MobileComm/M.1431523087.A.802.html', verify=False)
soup = BeautifulSoup(res.text)
mc = soup.select('#main-content')[0]
[i.decompose() for i in mc.select('span')]
[i.decompose() for i in mc.select('div')]
print mc
```

```py
import requests
from bs4 import BeautifulSoup
res = requests.get('http://www.ipeen.com.tw/shop/70978-%E5%8F%B0%E7%81%A3%E9%81%94%E7%BE%8E%E6%A8%82')
soup = BeautifulSoup(res.text)
print soup.select('h1')[0].text.strip()
```

```py
import requests
from bs4 import BeautifulSoup
res = requests.get('http://www.ipeen.com.tw/shop/70978-%E5%8F%B0%E7%81%A3%E9%81%94%E7%BE%8E%E6%A8%82')
soup = BeautifulSoup(res.text)
print soup.select('.info h1')
```

```py
import requests
header  = {'User-Agent':'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/43.0.2357.81 Safari/537.36',
'Cookie':'ASP.NET_SessionId=vkwwsfz4wjg3ej2o24mvac55; _ga=GA1.3.1009046832.1433175011; _gat=1; JubFrm-pagebox=%5EcourtFullName%3DTPDV%60%5Edy1%3D104%5Edm1%3D1%5Edd1%3D1%5Edy2%3D104%5Edm2%3D6%5Edd2%3D1%5Ejcatagory%3D0%5Eissimple%3D-1; y=p=VOUqAM8JSOHlsY%2byfpFRE3vG5r1FlWQQ%2bOuqDXyVne7gOxzydtBd08ujzgKglcOj; x=j=ExKZIgGGiQD19KUxjSulmQb58Xq9tzxh/6k2CZDMXMbqn+pyISjsF42Gyv8r7WXOK2TSUk8PjqCjFgL8iLOg+rhTDZRS/LF6A3yiOorRKBbqqfvT9nsAos6HffvejAESNTDqD1WguB6T7rQNSzOeq5kex9NSLQZUItrs84CqGL2v06p7VA8mxEwzCTHWAR1K2hgLnuTA4OwNIi895B4ZUOdZk4RVXu3nRwXQ+8uEnsY508rOpEzUh+n3Q72p/Dks3lfSuQ/0YF+ZtIfJegXo0b1HyufYOYRQsLZkW05Racc=',    
}
res = requests.get('http://fyjud.lawbank.com.tw/content3.aspx?p=tCqhCVJajSdSvECB24EhgQGmqpHQnLGUaI76sp7fUFw%3d', headers = header)
print res.text
```

```py
import requests
import re
res = requests.get('http://odds.zso8.com/html/match.html?t=20150624225319')
res.encoding = 'utf-8'
m = re.findall('m\[\d+\]=\"(.*?)\".split\(\',\'\);', res.text)
for ele in m:
    print ele.split(',')[9]
```

```py
import requests
from bs4 import BeautifulSoup
res = requests.get('http://tw.taobao.com/product/%E5%A4%9A%E6%A8%A3%E5%B1%8B-%E8%91%AB%E8%98%86-%E4%BF%9D%E6%BA%AB%E6%9D%AF.htm')
soup = BeautifulSoup(res.text)
for item in soup.select('.item'):
    print item.select('strong')[0].text
```

```py
# -*- coding: utf-8 -*-
import requests
from bs4 import BeautifulSoup
res = requests.get('http://www.tdcc.com.tw/smWeb/QryStock.jsp?SCA_DATE=20150724&SqlMethod=StockNo&StockNo=2330&StockName=&sub=%ACd%B8%DF')
soup  = BeautifulSoup(res.text)
stock = []
table = soup.select('.mt')[1]
for ele in table.select('tr')[1:]:
    td = ele.select('td')
    print td[0].text.encode('utf-8'), td[1].text.encode('utf-8') td[2].text.encode('utf-8'), \
          td[3].text.encode('utf-8'), td[4].text.encode('utf-8')
```

```py
import requests
from bs4 import BeautifulSoup as bs
res = requests.get('https://tw.bid.yahoo.com/tw/show/auctions?userID=Y6216276935&u=%3AY6216276935&clf=1#bd')
soup = bs(res.text)
for ele in soup.select('#yaubhil li'):
    print ele.select('a')[0]['href'] , ele.select('a img')[0]['title'], ele.select('a img')[0]['src']
```

```py
# -*- coding: utf-8 -*- 
from selenium import selenium 
from selenium import webdriver
from selenium.common.exceptions import NoSuchElementException
from selenium.webdriver.common.keys import Keys
import time
from bs4 import BeautifulSoup

browser = webdriver.Firefox() 

soup = BeautifulSoup(browser.page_source)
while len(soup.select('.pager-right')) > 0:
	for ele in soup.select('.hotel-info'):
		print "%s\t%s\t%s"%(ele.select('h3 a')[0].text, ele.select('h3 i')[0].text, ele.select('.property-snippet strong')[0].text.strip())
	browser.find_element_by_link_text("Next").click()
	soup = BeautifulSoup(browser.page_source)
	time.sleep(1)

browser.close()
```

```py
import requests
import json
import math
n = 963
pages = int(math.ceil(float(n)/10))
for page in range(1, pages+1):
    res = requests.get('http://www.agoda.com/api/zh-tw/Main/GetReviewComments?HotelId=908412&travellerTypeId=0&pageSize=10&pageNo={}&sortingOption=1'.format(page))
    jd = json.loads(res.text)
    for review in json.loads(res.text)['Comments']:
        print review['ReviewTitle']
```

```py
import requests
from bs4 import BeautifulSoup
import base64
payload = {}
res = requests.get('http://www.twse.com.tw/ch/trading/exchange/BWIBBU/BWIBBU_d.php')
res.encoding = 'big5'
soup = BeautifulSoup(res.text)
for input_tag in soup.select('input'):
    if input_tag.get('type') == 'hidden':
        payload[input_tag.get('name')] = base64.b64encode(input_tag.get('value').encode('utf-8'))
#print payload['html']
res2 = requests.post('http://www.twse.com.tw/ch/trading/exchange/BWIBBU/BWIBBU_print.php?language=ch&save=csv', data=payload, stream=True)
from shutil import copyfileobj

f = open('export2.csv', 'wb')
copyfileobj(res2.raw, f)
f.close()
```

```py
import requests
from bs4 import BeautifulSoup as bs
rs = requests.session()

payload = {
'pageTypeHidden':'1',
'code':'1101',
'ctl00$ContentPlaceHolder1$DropDownList1':'2015年6月'
}
res = rs.get('http://www.cnyes.com/twstock/directorholder/1101.htm')

#print res.text

soup = bs(res.text)
hidden = soup.select('input')
for hid in hidden:
    if hid.get('value') is not None and hid.get('value') != '':
        payload[hid.get('name')] = hid.get('value')

res2 = rs.post('http://www.cnyes.com/twstock/directorholder/1101.htm', data= payload)
soup = bs(res2.text)

table = soup.select('.tabvl')[0]
for tr in table.select('tr')[1:]:
    for td in tr.select('td'):
        print td.text,
    print
```

```py
from bs4 import BeautifulSoup as bs
html = """
<div class="aaa">
	Hello,how are you?
     <a>wwww</a>
     <strong>yyy</strong>
      I'am fine.
</div>
"""
soup = bs(html, 'html5lib')
soup.strong.decompose()
soup.a.decompose()
print soup.text
```

```py
import requests
import pandas
import json

url = 'http://ecshweb.pchome.com.tw/search/v3.3/all/results?q=%E5%88%AE%E9%AC%8D%E5%88%80&page=2&sort=rnk/dc'

r = requests.get(url)
jd = json.loads(r.text)
df = pandas.DataFrame(jd['prods'])
df.head()
```






