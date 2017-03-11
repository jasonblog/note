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
 
