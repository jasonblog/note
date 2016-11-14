# fb 社團爬標題發文者內容

```py
#! /usr/bin/python
# -*- coding: utf-8 -*-
import facebook
import pprint

token = 'EAACEdEose0cBALmqhAIsJnRF5aV5CUktHYoD0P3kYYqYArT50CZC85cKExotViEoZCy1ZCkyGFoh7ZBOchetDIcZAA2uq3WqhuXKqjo5K3UW2FcZAv5HELEsU8ZBxpRO4L4yRrJQaH84XrTAhzSoxAAsf4F367jb18Q7vDe0xqvkAZDZD'

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