# coding=UTF-8

import time
from datetime import datetime
import requests
import time
import sys
from bs4 import BeautifulSoup
requests.packages.urllib3.disable_warnings()


Board = ''
payload={
'from':'/bbs/'+ Board +'/index.html',
'yes':'yes' 
}

def getPageNumber(content) :
    startIndex = content.find('index')
    endIndex = content.find('.html')
    pageNumber = content[startIndex+5 : endIndex]
    return pageNumber

# python PttCrawler.py Gossiping 2 
if __name__ == "__main__":
    Board = str(sys.argv[1])
    ParsingPage = int(sys.argv[2])
    print 'Start parsing ['+ Board +']....'
    start_time = time.time()
    rs = requests.session()
    #八卦版18禁
    res = rs.post('https://www.ptt.cc/ask/over18',verify = False, data = payload)
    res = rs.get('https://www.ptt.cc/bbs/'+ Board +'/index.html',verify = False)
    soup = BeautifulSoup(res.text,'html.parser')
    ALLpageURL = soup.select('.btn.wide')[1]['href']
    ALLpage = int(getPageNumber(ALLpageURL)) + 1
    print 'Total pages:', ALLpage
    URLlist=[]
    fileName='PttData-'+ Board + '-' + datetime.now().strftime('%Y%m%d%H%M%S')+'.txt'

    #得到每頁的所有文章
    for index in range(ALLpage, ALLpage-int(ParsingPage), -1):
        url = 'https://www.ptt.cc/bbs/'+ Board +'/index'+ str(index) +'.html'
        res = rs.get(url, verify = False)
        soup = BeautifulSoup(res.text,'html.parser')
        UrlPer = []
        for entry in soup.select('.r-ent'):
            atag = entry.select('.title')[0].find('a') 
	    if(atag != None):
               URL = atag['href']          
               UrlPer.append('https://www.ptt.cc' + URL)   
        #需要反轉,因為網頁版最下面才是最新的文章
        for URL in reversed(UrlPer):
            URLlist.append(URL)   
    
    strNext = u"\n\n\n\n***************下一篇***************\n\n\n\n\n";
    content = ''
    #得到每篇文章的內容
    for URL in URLlist:
        res = rs.get(URL, verify = False)
        soup = BeautifulSoup(res.text, 'html.parser')
        data = soup.select('.bbs-screen.bbs-content')[0].text
        content += (data + strNext)
        time.sleep(0.05) 
       
    with open(fileName,'wb') as f:
        f.write( content.encode('utf8') )  
     
    print u'====================END===================='
    print u'execution time:' + str(time.time() - start_time)+'s'
