# 臺鐵資料下載



```py
#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys, random, os, shlex
import urllib, urllib2
from cookielib import CookieJar
from subprocess import check_call

INIT_URL = 'http://railway.hinet.net/check_ctno1.jsp'
IMAGE_URL = 'http://railway.hinet.net/ImageOut.jsp?pageRandom=%.16f'
AUDIO_URL = 'http://railway.hinet.net/PronounceRandonNumber.do?pageRandom=%.16f'

def init_session():
    params = {
        'person_id': '', 'from_station': '004', 'to_station': '004',
        'getin_date': '2016/08/24-00', 'train_no:': '', 'order_qty_str': '1',
        't_order_qty_str': '0', 'n_order_qty_str': '0', 'd_order_qty_str': '0',
        'b_order_qty_str': '0', 'z_order_qty_str': '0', 'returnTicket': '0'
    }
    opener.open(urllib2.Request(INIT_URL, urllib.urlencode(params))).read()

def covert_audio_encode(path):
    #print os.getcwd()
    prevdir = os.getcwd()
    os.chdir(path)
    #print os.getcwd()
    #raw_input()
    for filename in os.listdir("./"):
	if filename.endswith(".wav"):
	    #body, ext = os.path.splitext(filename)
	    command = 'ffmpeg -i ' + filename + ' tmp_file.wav' 
	    check_call(shlex.split(command))
	    command = 'mv' + ' tmp_file.wav ' + filename
	    check_call(shlex.split(command))

    os.chdir(prevdir)
    #print os.getcwd()

def save_data(url, output):
    headers = {'referer': INIT_URL}
    data = opener.open(urllib2.Request(url, None, headers)).read()
    if len(output) > 0:
        print output, url
        f = open(output, 'wb')
        f.write(data)

def read_arguments():
    if len(sys.argv) < 2 or (sys.argv[1] != 'test' and sys.argv[1] != 'train'):
        print 'usage: ./download_data.py test [count]'
        print '   or: ./download_data.py train [count]'
        return 0
    sys.argv[1] = 'T%s' % sys.argv[1][1:]
    if len(sys.argv) == 3:
        return int(sys.argv[2])
    return 10

def main():
    # arguments
    count = read_arguments()
    if count == 0:
        exit()
    # main
    global opener
    cj = CookieJar()
    opener = urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
    init_session()

    train_directory = "./Data/Train/"
    if not os.path.exists(train_directory):
	os.makedirs(train_directory)

    test_directory = "./Data/Test/"
    if not os.path.exists(test_directory):
	os.makedirs(test_directory)

    for i in range(count):
        rand = random.random()
        save_data(IMAGE_URL % rand, 'Data/%s/%d.jpg' % (sys.argv[1] ,i))
        save_data(AUDIO_URL % rand, 'Data/%s/%d.wav' % (sys.argv[1] ,i))

    path = 'Data/%s' % (sys.argv[1])
    covert_audio_encode(path)

if __name__ == '__main__':
    main()
```