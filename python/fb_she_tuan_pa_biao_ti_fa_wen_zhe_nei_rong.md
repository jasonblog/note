# fb 社團爬標題發文者內容

## Graph API Explorer

```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import facebook
import pprint

token = ''

graph=facebook.GraphAPI(token)
groups = graph.get_connections(id='me', connection_name='groups')['data']
group_id = [group['id'] for group in groups if group['name'] == 'Jason_Test'][0]
#print group_id
feed = graph.request('/%s/feed' % group_id, args = {'limit': 1000})
#pprint.pprint(feed)

for post in feed['data']:
    #pprint.pprint(post)
    if 'message' in post.keys():
        if 'comments' in post.keys():
            print post['message']
            for i in range(len(post['comments']['data'])):
                print post['comments']['data'][i]['from']['name']
                print post['comments']['data'][i]['created_time']
                print post['comments']['data'][i]['message']
        else:
            pass
            #print "沒+++"
            #print post['from']['name']
            #pprint.pprint(post)
            #print post['comments']
            #post_id = post['id']
            #print post_id
    else:
        pass
        #print "======================="
        #pprint.pprint(post)
```


## Jason app 帳號

```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import facebook
import pprint

token = ''

graph=facebook.GraphAPI(token)
groups = graph.get_connections(id='me', connection_name='groups')['data']
group_id = [group['id'] for group in groups if group['name'] == 'Jason_Test'][0]
#print group_id
feed = graph.request('/%s/feed' % group_id, args = {'limit': 1000, 'fields' : "message,comments"})
#feed = graph.request('/%s/feed' % group_id, args = {'comments':'', 'message':''})
#pprint.pprint(feed)

for post in feed['data']:
    #pprint.pprint(post)
    if 'message' in post.keys():
        if 'comments' in post.keys():
            print post['message']
            for i in range(len(post['comments']['data'])):
                print post['comments']['data'][i]['from']['name']
                print post['comments']['data'][i]['created_time']
                print post['comments']['data'][i]['message']
        else:
            pass
            #print "沒+++"
            #print post['from']['name']
            #pprint.pprint(post)
            #print post['comments']
            #post_id = post['id']
            #print post_id
    else:
        pass
        #print "======================="
        #pprint.pprint(post)

```

## Jason App 延長 token

```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import facebook
import pprint
import requests

def extend_access_token(access_token):
    app_id = ""                       # Obtained from https://developers.facebook.com/        
    client_secret = ""         # Obtained from https://developers.facebook.com/

    link = "https://graph.facebook.com/oauth/access_token?grant_type=fb_exchange_token&client_id=" + app_id +"&client_secret=" + client_secret + "&fb_exchange_token=" + access_token
    s = requests.Session()
    token = s.get(link).content
    token = token.split("&")[0]                 # this strips out the expire info (now set set about 5184000 seconds, or 60 days)
    token = token.strip("access_token=")        # Strips out access token
    print token


def main():
    token = ''

    extend_access_token(token)
    graph=facebook.GraphAPI(token)
    groups = graph.get_connections(id='me', connection_name='groups')['data']
    group_id = [group['id'] for group in groups if group['name'] == 'Jason_Test'][0]
    feed = graph.request('/%s/feed' % group_id, args = {'limit': 1000, 'fields' : "message,comments"})
    #pprint.pprint(feed)

    for post in feed['data']:
	#pprint.pprint(post)
	if 'message' in post.keys():
	    if 'comments' in post.keys():
		print post['message']
		for i in range(len(post['comments']['data'])):
		    print post['comments']['data'][i]['from']['name']
		    print post['comments']['data'][i]['created_time']
		    print post['comments']['data'][i]['message']
	    else:
		pass
		#print "沒+++"
		#print post['from']['name']
		#pprint.pprint(post)
		#print post['comments']
		#post_id = post['id']
		#print post_id
	else:
	    pass
	    #print "======================="
	    #pprint.pprint(post)

if __name__ == "__main__":
    main()
```

## update

```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import facebook
import pprint
import requests

def extend_access_token(access_token):
    app_id = ""                       # Obtained from https://developers.facebook.com/        
    client_secret = ""         # Obtained from https://developers.facebook.com/

    link = "https://graph.facebook.com/oauth/access_token?grant_type=fb_exchange_token&client_id=" + app_id +"&client_secret=" + client_secret + "&fb_exchange_token=" + access_token
    s = requests.Session()
    token = s.get(link).content
    token = token.split("&")[0]                 # this strips out the expire info (now set set about 5184000 seconds, or 60 days)
    token = token.strip("access_token=")        # Strips out access token
    #print token


def main():
    token = ''

    extend_access_token(token)
    graph=facebook.GraphAPI(token)
    groups = graph.get_connections(id='me', connection_name='groups')['data']
    group_id = [group['id'] for group in groups if group['name'] == 'Jason_Test'][0]
    feed = graph.request('/%s/feed' % group_id, args = {'limit': 1000,'fields': "message,comments.fields(comments,likes,from,id,message,user_likes,created_time,like_count)"})

    #pprint.pprint(feed)


    for post in feed['data']:
	#pprint.pprint(post)
        #print '--------------------------------------------------------------'
	if 'message' in post.keys():
	    if 'comments' in post.keys():
		print (u'標題:%s' % post['message'])
		#for i in range(len(post['comments']['data'])):
		for p in post['comments']['data']:
                    print(u'日期:%s ID:%s 名字:%s 內容:%s' % \
		         (p['created_time'], \
		         p['from']['id'], \
		         p['from']['name'], \
		         p['message']))
                    if 'comments' in p.keys():
                        #pprint.pprint(post['comments']['data'][i]['comments'])
                        for pp in p['comments']['data']:
                            #print post['comments']['data'][i]['comments']['data'][j]['message']
                            print(u'日期:%s ID:%s 名字:%s 內容:%s' % \
                                 (pp['created_time'], \
                                  pp['from']['id'], \
                                  pp['from']['name'], \
                                  pp['message']))
                        
		print '--------------------------------------------------------------'
	    else:
		pass
	else:
	    pass
	    #print "======================="
	    #pprint.pprint(post)
    raw_input()

if __name__ == "__main__":
    main()
```


## 內容導向檔案
```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import facebook
import pprint
import requests
import sys
import codecs

def extend_access_token(access_token):
    app_id = ""                       # Obtained from https://developers.facebook.com/        
    client_secret = ""         # Obtained from https://developers.facebook.com/

    link = "https://graph.facebook.com/oauth/access_token?grant_type=fb_exchange_token&client_id=" + app_id +"&client_secret=" + client_secret + "&fb_exchange_token=" + access_token
    s = requests.Session()
    token = s.get(link).content
    token = token.split("&")[0]                 # this strips out the expire info (now set set about 5184000 seconds, or 60 days)
    token = token.strip("access_token=")        # Strips out access token
    #print token

def main():
    token = ''
    extend_access_token(token)
    graph=facebook.GraphAPI(token)
    groups = graph.get_connections(id='me', connection_name='groups')['data']
    group_id = [group['id'] for group in groups if group['name'] == 'Jason_Test'][0]
    feed = graph.request('/%s/feed' % group_id, args = {'limit': 1000,'fields': "message,comments.fields(comments,likes,from,id,message,user_likes,created_time,like_count)"})
    #pprint.pprint(feed)

    stdout_backup = sys.stdout
    log_file = codecs.open("message.log", "w", "utf-8")
    sys.stdout = log_file

    for post in feed['data']:
	#pprint.pprint(post)
        #print '--------------------------------------------------------------'
	if 'message' in post.keys():
	    if 'comments' in post.keys():
		print (u'標題:%s' % post['message'])
		#for i in range(len(post['comments']['data'])):
		for p in post['comments']['data']:
                    print(u'日期:%s ID:%s 名字:%s 內容:%s' % \
		         (p['created_time'], \
		         p['from']['id'], \
		         p['from']['name'], \
		         p['message']))
                    if 'comments' in p.keys():
                        #pprint.pprint(post['comments']['data'][i]['comments'])
                        for pp in p['comments']['data']:
                            #print post['comments']['data'][i]['comments']['data'][j]['message']
                            print(u'日期:%s ID:%s 名字:%s 內容:%s' % \
                                 (pp['created_time'], \
                                  pp['from']['id'], \
                                  pp['from']['name'], \
                                  pp['message']))
                        
		print '--------------------------------------------------------------'

    log_file.close()
    sys.stdout = stdout_backup
    print "Done."
    raw_input()

if __name__ == "__main__":
    main()

```


## HTML

```py
from html import HTML
inline_css={
           'class1':'color:#00FF00;',
           'class2':'color:#FF0000;',
           'class3':'color:#FFFF00;',
}

b = HTML()
t = b.table(border='1')
r = t.tr()
r.td('column 1', style=inline_css['class1'])
r.td('column 2', style=inline_css['class2'])
r.td('column 3', style=inline_css['class3'])
print str(b)
```