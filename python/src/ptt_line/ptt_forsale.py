#coding=utf-8 
from PttWebCrawler.crawler import PttWebCrawler as crawler
import codecs, json, os, sys
import requests
import moment, time

def line_nofity(item):
    url = "https://notify-api.line.me/api/notify"
    token = "KXwzqEGtIp1JEkS5GjqXqRAT0D4BdQQvCNcqOa7ySfz"
    headers = {"Authorization" : "Bearer "+ token}

    message = item['date'] + "\n" + item[u'article_title'] + "\n" + item[u'author'] +  "\n" + \
              item[u'content'] + "\n" + item[u'content'] + "\n" + item[u'url'] + "\n"
    #message = item[u'b_作者'] + "\n"
    payload = {"message" :  message}
    r = requests.post(url ,headers = headers ,params=payload)

if __name__ == "__main__":  
    KeyWord = (sys.argv[1])

    # (設為負數則以倒數第幾頁計算) 
    ptt = crawler(['-b', 'forsale', '-i', '-1', '2'])
    #filename = 'forsale--1-5.json'
    print(ptt.json_filename)
    with codecs.open(ptt.json_filename, 'r', encoding='utf-8') as f:
    #with open('forsale-0-2.json', 'r') as f:
        json_data = json.load(f)

        for item in json_data['articles']:
            if (KeyWord in item[u'article_title'].lower() or \
                KeyWord in item[u'content'].lower()) and \
                (u'看板規則' not in item[u'content'].lower() and \
                 u'公告' not in item[u'article_title'].lower() and \
                 u'市集' not in item[u'article_title'].lower()):

                diff_time = moment.utc(item[u'date']) - moment.utc(str(time.asctime(time.gmtime(time.time()))))
                diff_time = diff_time.seconds / 60.0
                #print(diff_time)
                if(diff_time < 25) :  
                    line_nofity(item)
                    print(item[u'article_title'])
                    print(item[u'author'])
                    print(item[u'content'])
                    print(item[u'date'])
                    print(item[u'url'])
                    print('\n')

    os.remove(ptt.json_filename)
