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
    app_id = "1240481449344198"                        # Obtained from https://developers.facebook.com/        
    client_secret = "2faea3e8333af28fb24d123a44383550" # Obtained from https://developers.facebook.com/

    link = "https://graph.facebook.com/oauth/access_token?grant_type=fb_exchange_token&client_id=" \
           + app_id +"&client_secret=" \
	   + client_secret \
	   + "&fb_exchange_token=" \
	   + access_token

    s = requests.Session()
    token = s.get(link).content
    token = token.split("&")[0]                 # this strips out the expire info (now set set about 5184000 seconds, or 60 days)
    token = token.strip("access_token=")        # Strips out access token
    print token


def main():
    token = 'EAARoNhgjfMYBABe6tBEsJCW7ccXblCbrlaS3Pr0MVFtjuT5LqbDJOmzjZCk3ZABjZCm55f0S5ATydbr8YmKS7AnRag9DzFZCtKK4CR0Xudsl32Jke3iozBTggsNnm2paxId1hLCDFkvxzVcS1P2tUDDPZA0ZBJc3SPUziXxXp6tgZDZD'

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
		print (u'標題:%s' % post['message'])
		for i in range(len(post['comments']['data'])):
                    print(u'日期:%s 名字:%s 內容:%s' % \
			 (post['comments']['data'][i]['created_time'], \
			  post['comments']['data'][i]['from']['name'], \
			  post['comments']['data'][i]['message']))
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