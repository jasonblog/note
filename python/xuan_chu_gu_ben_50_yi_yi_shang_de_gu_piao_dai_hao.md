# 選出股本50億以上的股票代號


```py
 # coding=UTF-8
#藉由網站選股條件先選出股本50億以上的股票代號
import math
import requests
import re  #正規表示式
from bs4 import BeautifulSoup
# 下載網頁內容
header = {'user-agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36'}

VAL_S = 8   #股本範圍起
VAL_E = 15    #股本範圍迄

rt = requests.get('https://goodinfo.tw/StockInfo/StockList.asp?MARKET_CAT=%E8%87%AA%E8%A8%82%E7%AF%A9%E9%81%B8&INDUSTRY_CAT=%E6%88%91%E7%9A%84%E6%A2%9D%E4%BB%B6&FILTER_ITEM0=%E8%82%A1%E6%9C%AC+%28%E5%84%84%E5%85%83%29&FILTER_VAL_S0=' + str(VAL_S) + '&FILTER_VAL_E0=' + str(VAL_E), headers = header)

# 確認是否下載成功
if rt.status_code == requests.codes.ok:
  # 以 BeautifulSoup 解析 HTML 程式碼
  soup = BeautifulSoup(rt.text, 'html.parser')
  
  #<td style="mso-number-format:\@;"><nobr><a class="link_black" href="StockDetail.asp?STOCK_ID=1101"...
  data = soup.find_all('td', style='mso-number-format:\@;')
  
  #使用正規表示式找出所有四位數的股票代號
  m = re.findall('\d{4}', str(data))
  
  #set:去除重複，sorted:排序
  STOCK_ID = sorted(set(m))
  
  #print(len(STOCK_ID))

sIDx = [[0 for i in range(9)] for j in range(9)]    #2維陣列(9X9)

# math.ceil() 無條件進位。因為一次爬超過120幾筆會被網站認為攻擊，所以分次爬
for index in range( math.ceil(len(STOCK_ID)/100) ):
  sIDx[index] = STOCK_ID[index*100 : (index+1)*100 - 1]

# for sID in STOCK_ID:
  # r = requests.get('https://goodinfo.tw/StockInfo/ShowK_Chart.asp?STOCK_ID=' + str(sID) + '&CHT_CAT2=WEEK', headers = header)
  # soup = BeautifulSoup(r.text, 'html.parser')
  # if soup.find_all('tr', id='row14') == []:
    # print(sID)

    

chosen = []
week_s = 1
week_e = 14
grow = 5    #成交量倍數

for y in range( math.ceil(len(STOCK_ID)/100) ):
  for sID in sIDx[y]:
    r = requests.get('https://goodinfo.tw/StockInfo/ShowK_Chart.asp?STOCK_ID=' + str(sID) + '&CHT_CAT2=WEEK', headers = header)
    soup = BeautifulSoup(r.text, 'html.parser')   # 以 BeautifulSoup 解析 HTML 程式碼
    
    volume = []

    if soup.find_all('tr', id='row' + str(week_e)) != []:
      print(sID)
      for i in range(week_s, week_e):
      
        #<tr align="center" bgcolor="#EDEDED" height="23px" id="row1" onmouseout="ChgBGColor(['hrow1','row1'],'#EDEDED');" onmouseover="ChgBGColor(['hrow1','row1'],'#fff2cc');"> <td title="2018/7/2~2018/7/6"><nobr>W1827</nobr></td><td><nobr>5</nobr></td> <td style="color:green"><nobr>33.45</nobr></td> <td style="color:red"><nobr>34.8</nobr></td> <td style="color:green"><nobr>33.4</nobr></td> <td style="color:red"><nobr>34.2</nobr></td> <td style="color:red"><nobr>+0.7</nobr></td> <td style="color:red"><nobr>+2.09%</nobr></td> <td><nobr>41.9</nobr></td> <td><nobr>8.4</nobr></td> <td><nobr>14.3</nobr></td> <td><nobr>2.9</nobr></td> <td style="color:green"><nobr>-1.96</nobr></td> <td style="color:red"><nobr>+0.92</nobr></td> <td style="color:red"><nobr>+0.12</nobr></td> <td style="color:green"><nobr>-0.92</nobr></td> <td><nobr>22.9</nobr></td> <td style="color:green"><nobr>-0.42</nobr></td> <td>2.04</td> <td style="color:black"><nobr>0</nobr></td> <td>0.06</td> <td><nobr>2.8</nobr></td></tr>
        data = soup.find_all('tr', id='row' + str(i))

        #使用正規表示式找出數字區
        m = re.findall('<nobr>[+-]?\d+[.]?\d*</nobr>', str(data))

        #第7個剛好是成交張數
        #print(m[6])
        volume.append(m[6][6:-7])
      
      print(volume)

      #季均量=前13周成交量平均(含本周)
      tmp = 0
      for x in volume:
        tmp = tmp + float(x)

      seasonAVG = tmp / len(volume)
      
      if float(volume[0]) > (seasonAVG * grow):   #週量大於季均量x倍數(爆量)
      #if float(volume[0]) < seasonAVG:       #週量小於季均量(量縮)
        chosen.append(sID)

print('chosen')
print(chosen)
```