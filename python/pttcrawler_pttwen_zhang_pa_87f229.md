

# PTTcrawler (PTT文章爬蟲)


```py
#coding=utf-8 
import re
import sys
import json
import requests
import io
import random
from time import sleep
from datetime import datetime
from bs4 import BeautifulSoup  
requests.packages.urllib3.disable_warnings()

PttName=""
load={
'from':'/bbs/'+PttName+'/index.html',
'yes':'yes' 
}

rs=requests.session()
res=rs.post('https://www.ptt.cc/ask/over18',verify=False,data=load)
FILENAME=""

def PageCount(PttName):
    res=rs.get('https://www.ptt.cc/bbs/'+PttName+'/index.html',verify=False)
    soup=BeautifulSoup(res.text,'html.parser')
    ALLpageURL = soup.select('.btn.wide')[1]['href']
    ALLpage=int(getPageNumber(ALLpageURL))+1
    return  ALLpage 

def crawler(PttName,ParsingPage):
        ALLpage=PageCount(PttName)
        g_id = 0;
 
	for number  in range(ALLpage, ALLpage-int(ParsingPage),-1):
            res=rs.get('https://www.ptt.cc/bbs/'+PttName+'/index'+str(number)+'.html',verify=False)
            soup = BeautifulSoup(res.text,'html.parser')
	    for tag in soup.select('div.title'):
		try:
                    atag=tag.find('a')
                    time=random.uniform(0, 1)/5
                    #print 'time:',time
                    sleep(time)
                    if(atag):
                       URL=atag['href']   
                       link='https://www.ptt.cc'+URL
                       #print link
	               g_id = g_id+1
		       parseGos(link,g_id)                     
		except:
                    print 'error:',URL
 
def parseGos(link , g_id):
        res=rs.get(link,verify=False)
        soup = BeautifulSoup(res.text,'html.parser')
        # author
        author  = soup.select('.article-meta-value')[0].text
        #author = soup.find("span", {'class': 'article-meta-value'}).text              
        #print 'author:',author
        # title
        title = soup.select('.article-meta-value')[2].text
        #print 'title:',title
        # date
        date = soup.select('.article-meta-value')[3].text
        #print 'date:',date
        # ip       
        try:
                targetIP=u'※ 發信站: 批踢踢實業坊'
                ip =  soup.find(string = re.compile(targetIP))
                ip = re.search(r"[0-9]*\.[0-9]*\.[0-9]*\.[0-9]*",ip).group()
        except:
                ip = "ip is not find"
        #print 'ip:',ip

        # content
        content = soup.find(id="main-content").text
        target_content=u'※ 發信站: 批踢踢實業坊(ptt.cc),'
        content = content.split(target_content)
        content = content[0].split(date)
        main_content = content[1].replace('\n', '  ').replace('\t', '  ')
        #print 'content:',main_content
        
        # message
        num , g , b , n ,message = 0,0,0,0,{}
        for tag in soup.select('div.push'):
                num += 1
                push_tag = tag.find("span", {'class': 'push-tag'}).text
                #print "push_tag:",push_tag
                push_userid = tag.find("span", {'class': 'push-userid'}).text       
                #print "push_userid:",push_userid
                push_content = tag.find("span", {'class': 'push-content'}).text   
                push_content = push_content[1:]
                #print "push_content:",push_content
                push_ipdatetime = tag.find("span", {'class': 'push-ipdatetime'}).text   
                push_ipdatetime = remove(push_ipdatetime, '\n')
                #print "push-ipdatetime:",push_ipdatetime 
                
                message[num]={"狀態":push_tag.encode('utf-8'),"留言者":push_userid.encode('utf-8'),
                              "留言內容":push_content.encode('utf-8'),"留言時間":push_ipdatetime.encode('utf-8')}
                if push_tag == u'推 ':
                        g += 1
                elif push_tag == u'噓 ':
                        b += 1
                else:
                        n += 1
  
        messageNum = {"g":g,"b":b,"n":n,"all":num}
        # json-data  type(d) dict
          
        d={ "a_ID":g_id , "b_作者":author.encode('utf-8'), "c_標題":title.encode('utf-8'), "d_日期":date.encode('utf-8'),
            "e_ip":ip.encode('utf-8'), "f_內文":main_content.encode('utf-8'), "g_推文":message,"h_推文總數":messageNum}
        json_data = json.dumps(d,ensure_ascii=False,indent=4,sort_keys=True)+','
	store(json_data) 

def store(data):
    with open(FILENAME, 'a') as f:
        f.write(data)
     
def remove(value, deletechars):
    for c in deletechars:
        value = value.replace(c,'')
    return value.rstrip();
   

def getPageNumber(content) :
    startIndex = content.find('index')
    endIndex = content.find('.html')
    pageNumber = content[startIndex+5 : endIndex]
    return pageNumber

if __name__ == "__main__":  
   PttName = str(sys.argv[1])
   ParsingPage = int(sys.argv[2])
   FILENAME='data-'+PttName+'-'+datetime.now().strftime('%Y-%m-%d-%H-%M-%S')+'.json'
   store('[') 
   print 'Start parsing [',PttName,']....'
   crawler(PttName,ParsingPage)
   store(']') 
   

   with open(FILENAME, 'r') as f:
        p = f.read()
   with open(FILENAME, 'w') as f:
        #f.write(p.replace(',]',']'))
        f.write(p[:-2]+']')   
```

A crawler for web PTT 

PTT文章爬蟲
* [Demo Video](https://www.youtube.com/watch?v=qq3kuDU3k50&feature=youtu.be) - Linux

特色
* 抓取PTT 文章

輸出格式
    輸出 .json，格式如下
 

    "a_ID": 編號,
    "b_作者": 作者名,
    "c_標題": 標題,
    "d_日期": 發文時間,
    "e_ip": 發文ip,
    "f_內文": 內文,
    "g_推文": {
        "推文編號": {
            "狀態": 推 or 噓 or →,
            "留言內容": 留言內容,
            "留言時間": 留言時間,
            "留言者": 留言者
        }
    },
    "h_推文總數": {
        "all": 推文數目,
        "b": 噓數,
        "g": 推數,
        "n": →數
    }
    
    
###執行環境
python 2.7.3

### 執行方法

    $ python pttcrawler.py [版名]  [抓取頁數]
    
### 範例
  爬PTT Gossiping版 2頁 文章內容

    $ python pttcrawler.py  Gossiping  2
    
  假設總共有100頁，則會爬取
  
  https://www.ptt.cc/bbs/Gossiping/index100.html 至 https://www.ptt.cc/bbs/Gossiping/index101.html 之間的內容。
    
---


###執行環境
python 3

### 執行方法

```sh
$ python pttcrawler.py [版名]  [抓取頁數]
```

```py
#coding=utf-8
import re
import sys
import json
import requests
import time
from datetime import datetime
from bs4 import BeautifulSoup


requests.packages.urllib3.disable_warnings()

PttName, fileName = "", ""
load = {
    'from': '/bbs/' + PttName + '/index.html',
    'yes': 'yes'
}

rs = requests.session()


def getPageNumber(content):
    startIndex = content.find('index')
    endIndex = content.find('.html')
    pageNumber = content[startIndex + 5: endIndex]
    return pageNumber


def over18(board):
    res = rs.get('https://www.ptt.cc/bbs/' + board + '/index.html', verify=False)
    # 先檢查網址是否包含'over18'字串 ,如有則為18禁網站
    if (res.url.find('over18') > -1):
        print("18禁網頁")
        load = {
            'from': '/bbs/' + board + '/index.html',
            'yes': 'yes'
        }
        res = rs.post('https://www.ptt.cc/ask/over18', verify=False, data=load)
        return BeautifulSoup(res.text, 'html.parser')
    return BeautifulSoup(res.text, 'html.parser')


def crawler(url_list):
    count, g_id = 0, 0
    total = len(url_list)
    # 開始爬網頁
    while url_list:
        url = url_list.pop(0)
        res = rs.get(url, verify=False)
        soup = BeautifulSoup(res.text, 'html.parser')
        # 如網頁忙線中,則先將網頁加入 index_list 並休息1秒後再連接
        if (soup.title.text.find('Service Temporarily') > -1):
            url_list.append(url)
            # print u'error_URL:', url
            # print u'error_URL head:', soup.title.text
            time.sleep(1)
        else:
            count += 1
            # print u'OK_URL:', url
            # print u'OK_URL head:', soup.title.text
            for r_ent in soup.find_all(class_="r-ent"):
                # 先得到每篇文章的篇url
                link = r_ent.find('a')
                if (link):
                    # 確定得到url
                    URL = 'https://www.ptt.cc' + link['href']
                    g_id = g_id + 1
                    # 避免被認為攻擊網站
                    time.sleep(0.1)
                    # 開始爬文章內容
                    parseGos(URL, g_id)
            print("download: " + str(100 * count / total) + " %.")
        # 避免被認為攻擊網站
        time.sleep(0.1)


def checkformat(soup, class_tag, data, index, link):
    # 避免有些文章會被使用者自行刪除 標題列 時間  之類......
    try:
        content = soup.select(class_tag)[index].text
    except Exception as e:
        print('checkformat error URL', link)
        # print 'checkformat:',str(e)
        content = "no " + data
    return content


def parseGos(link, g_id):
    res = rs.get(link, verify=False)
    soup = BeautifulSoup(res.text, 'html.parser')

    # author 文章作者
    # author  = soup.select('.article-meta-value')[0].text
    author = checkformat(soup, '.article-meta-value', 'author', 0, link)
    # print 'author:',author

    # title 文章標題  
    # title = soup.select('.article-meta-value')[2].text
    title = checkformat(soup, '.article-meta-value', 'title', 2, link)
    # print 'title:',title

    # date 文章日期
    # date = soup.select('.article-meta-value')[3].text
    date = checkformat(soup, '.article-meta-value', 'date', 3, link)
    # print 'date:',date

    # ip 文章文章ip       
    try:
        targetIP = u'※ 發信站: 批踢踢實業坊'
        ip = soup.find(string=re.compile(targetIP))
        ip = re.search(r"[0-9]*\.[0-9]*\.[0-9]*\.[0-9]*", ip).group()
    except:
        ip = "ip is not find"
    # print 'ip:',ip

    # content  文章內文
    try:
        content = soup.find(id="main-content").text
        target_content = u'※ 發信站: 批踢踢實業坊(ptt.cc),'
        content = content.split(target_content)
        content = content[0].split(date)
        main_content = content[1].replace('\n', '  ')
        # print 'content:',main_content
    except Exception as e:
        main_content = 'main_content error'
        print('main_content error URL' + link)
        # print 'main_content error:',str(e)

    # message 推文內容
    num, g, b, n, message = 0, 0, 0, 0, {}

    for tag in soup.select('div.push'):
        try:
            # push_tag  推文標籤  推  噓  註解(→)
            push_tag = tag.find("span", {'class': 'push-tag'}).text
            # print "push_tag:",push_tag

            # push_userid 推文使用者id
            push_userid = tag.find("span", {'class': 'push-userid'}).text
            # print "push_userid:",push_userid

            # push_content 推文內容
            push_content = tag.find("span", {'class': 'push-content'}).text
            push_content = push_content[1:]
            # print "push_content:",push_content

            # push-ipdatetime 推文時間
            push_ipdatetime = tag.find("span", {'class': 'push-ipdatetime'}).text
            push_ipdatetime = push_ipdatetime.rstrip()
            # print "push-ipdatetime:",push_ipdatetime

            num += 1
            message[num] = {"狀態": push_tag, "留言者": push_userid,
                            "留言內容": push_content, "留言時間": push_ipdatetime}

            # 計算推噓文數量 g = 推 , b = 噓 , n = 註解
            if push_tag == u'推 ':
                g += 1
            elif push_tag == u'噓 ':
                b += 1
            else:
                n += 1
        except Exception as e:
            print("push error URL:" + link)
            # print "push error:",str(e)

    messageNum = {"g": g, "b": b, "n": n, "all": num}

    d = {"a_ID": g_id, "b_作者": author, "c_標題": title, "d_日期": date,
         "e_ip": ip, "f_內文": main_content, "g_推文": message, "h_推文總數": messageNum}

    # json.dumps 序列化時預設為對中文使用ascii編碼
    json_data = json.dumps(d, ensure_ascii=False, indent=4, sort_keys=True) + ','
    store(json_data)


def store(data):
    with open(fileName, 'a') as f:
        f.write(data.encode(sys.stdin.encoding, "replace").decode(sys.stdin.encoding))


if __name__ == "__main__":
    PttName, ParsingPage = str(sys.argv[1]), int(sys.argv[2])
    start_time = time.time()
    print('Start parsing ' + PttName + '....')
    fileName = 'data-' + PttName + '-' + datetime.now().strftime('%Y%m%d%H%M%S') + '.json'
    # 檢查看板是否為18禁,有些看板為18禁
    soup = over18(PttName)
    ALLpageURL = soup.select('.btn.wide')[1]['href']
    # 得到本看板全部的index數量
    ALLpage = int(getPageNumber(ALLpageURL)) + 1
    index_list = []
    for index in range(ALLpage, ALLpage - int(ParsingPage), -1):
        page_url = 'https://www.ptt.cc/bbs/' + PttName + '/index' + str(index) + '.html'
        index_list.append(page_url)

    store('[\n')
    crawler(index_list)

    # 移除最後一個  "," 號
    with open(fileName, 'r') as f:
        content = f.read()
    with open(fileName, 'w') as f:
        f.write(content[:-1] + "\n]")

    print("爬蟲結束...")
    print("execution time:" + str(time.time() - start_time) + "s")

```
