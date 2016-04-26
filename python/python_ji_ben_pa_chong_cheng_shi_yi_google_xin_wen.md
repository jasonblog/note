# Python 基本爬蟲程式 以 Google 新聞為例

```py
# coding=utf-8

import requests
from bs4 import BeautifulSoup

res = requests.get("https://news.google.com")
soup = BeautifulSoup(res.text)
print soup.select(".esc-body")

count = 1

for item in soup.select(".esc-body"):
    print '======[',count,']========='
    news_title = item.select(".esc-lead-article-title")[0].text
    news_url = item.select(".esc-lead-article-title")[0].find('a')['href']
    print news_title
    print news_url
    count += 1
```

```sh
======[ 1 ]=========
助台船繳錢獲釋外交部：非默認日主張
http://www.cna.com.tw/news/aipl/201604260398-1.aspx
======[ 2 ]=========
翁啟惠該走嗎？
http://www.appledaily.com.tw/realtimenews/article/new/20160426/847453/
======[ 3 ]=========
傳北韓核試就緒南韓：只等領導高層下令
http://news.ltn.com.tw/news/world/breakingnews/1676669
======[ 4 ]=========
國際原能總署：對核能安全勿過於自滿
http://www.cna.com.tw/news/ahel/201604260481-1.aspx
======[ 5 ]=========
常州學校土壤汙染案官方認錯並查處
http://www.cna.com.tw/news/acn/201604260390-1.aspx
======[ 6 ]=========
蒙娜麗莎模特兒義學者：有男有女
http://www.cna.com.tw/news/ahel/201604260477-1.aspx
======[ 7 ]=========
人質被殺加國總理初次處理恐怖主義危機
http://www.appledaily.com.tw/realtimenews/article/new/20160426/847355/
======[ 8 ]=========
軍武》法國擊敗日德贏得澳洲新潛艦訂單
http://www.nownews.com/n/2016/04/26/2077409
======[ 9 ]=========
歐盟調查：中國為其最大危險商品進口來源
http://www.thenewslens.com/post/317235/
======[ 10 ]=========
中國加強管控海外NGO 管理法草案三審中
http://news.ltn.com.tw/news/world/breakingnews/1676364
======[ 11 ]=========
軍人撫卹條例三讀通過「過勞死」列入因公死亡撫卹範疇
http://www.nownews.com/n/2016/04/26/2063608
======[ 12 ]=========
醫師過勞林奏延盼4年內落實住院醫師納《勞基法》
http://www.appledaily.com.tw/realtimenews/article/new/20160426/847399/
======[ 13 ]=========
不滿翁啟惠請假立院教委會可能不開會
http://www.cna.com.tw/news/aipl/201604260482-1.aspx
======[ 14 ]=========
市旗被嫌「全台最醜」 卓冠廷：市府該讓市旗相關法令明確化
http://news.ltn.com.tw/news/politics/breakingnews/1676972
======[ 15 ]=========
財報－中國汽車銷售冷現代汽車Q1利潤減11%
http://www.nownews.com/n/2016/04/26/2077783
======[ 16 ]=========
房市回溫？建築貸款餘額創4個月高點
http://www.nownews.com/n/2016/04/26/2077778
======[ 17 ]=========
Apple Pay登台？擬520前交行政院定奪
http://www.chinatimes.com/realtimenews/20160426005723-260410
======[ 18 ]=========
鋼鐵：鋼廠繼續大幅上調長材價格
http://news.cnyes.com/Content/20160426/20160426093028000240974.shtml
======[ 19 ]=========
骨幹網速增為100G 張善政籲「勿只看雜七雜八東西」
http://www.appledaily.com.tw/realtimenews/article/new/20160426/847243/
======[ 20 ]=========
網路取代不了書本偏鄉閱讀推動不能停
http://eradio.ner.gov.tw/news/?recordId=27697&_sp=detail
======[ 21 ]=========
上班打卡制，下班責任制！ 4成3上班族擔心自己會過勞
http://www.nownews.com/n/2016/04/26/2077269
======[ 22 ]=========
電動車話題夯統振旗下統達能源將合作切入大陸龐大市場
http://www.nownews.com/n/2016/04/26/2077777
======[ 23 ]=========
舉重》中國靠邊站！郭婞淳亞錦賽舉出2金1銀
http://sports.ltn.com.tw/news/breakingnews/1677193
======[ 24 ]=========
德甲／表現不如預期狼堡釋出本特納大帝
http://www.nownews.com/n/2016/04/26/2077152
======[ 25 ]=========
旅美球星／曹錦輝後援1局失3分
http://www.nownews.com/n/2016/04/26/2077052
======[ 26 ]=========
德甲／多特蒙德願讓隊長離隊也歡迎葛斯回家
http://www.nownews.com/n/2016/04/26/2077622
======[ 27 ]=========
性愛片網上瘋傳蛇精男哭認與女友錄着玩
http://tw.on.cc/cn/bkn/cnt/news/20160424/bkncn-20160424121626036-0424_05011_001.html
======[ 28 ]=========
侯佩岑出席活動大談媽媽經
https://video.udn.com/topic/557/479381
======[ 29 ]=========
林彥君日本出外景變臉狠狠打槍張立東
http://www.nownews.com/n/2016/04/26/2076319
======[ 30 ]=========
北捷電力異常象山至台北車站暫時停駛
http://www.appledaily.com.tw/realtimenews/article/new/20160426/847468/
======[ 31 ]=========
惡客不滿飛機延誤摑女地勤還囂張稱「又如何」（有片）
http://china.hket.com/article/1415150/?r=mcsdrs
======[ 32 ]=========
停棺逾半年未下葬新北最高罰5萬
http://www.appledaily.com.tw/realtimenews/article/new/20160426/847253/
======[ 33 ]=========
張志軍會北市里長參訪團強調基層交流
http://www.cna.com.tw/news/acn/201604260480-1.aspx
======[ 34 ]=========
江蘇漢報復鄰居斬六童再到鄰市血洗岳父家
http://tw.on.cc/cn/bkn/cnt/news/20160426/bkncn-20160426111759388-0426_05011_001.html
======[ 35 ]=========
入伍前夕玩過頭停車睡路邊飄K煙味遭逮
http://www.nownews.com/n/2016/04/26/2077380
======[ 36 ]=========
【影片】男子跳樓尋短竟壓死無辜賣菜阿婆
http://www.appledaily.com.tw/realtimenews/article/new/20160426/846955/
======[ 37 ]=========
校車翻覆學生繫妥安全帶保命
http://www.chinatimes.com/realtimenews/20160426002862-260402
======[ 38 ]=========
八仙案宣判法扶律師團發表4點聲明
http://udn.com/news/story/2/1654364
======[ 39 ]=========
什麼鬼學校！ 竟強迫愛滋病人退學
http://www.appledaily.com.tw/realtimenews/article/new/20160426/847279/
======[ 40 ]=========
普生旗下普懷醫學開幕推廣預防醫學守護民眾健康
http://www.nownews.com/n/2016/04/26/2077787
======[ 41 ]=========
網拍環境用藥最高罰30萬元
http://www.idn.com.tw/news/news_content.php?catid=4&catsid=2&catdid=0&artid=20160426abcd020
======[ 42 ]=========
女中尉結婚前上吊輕生家屬疑案情不單純
http://news.cts.com.tw/cts/society/201604/201604261744818.html
```