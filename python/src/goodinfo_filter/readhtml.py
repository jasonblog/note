#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import pandas as pd
from bs4 import BeautifulSoup
import codecs
import re
import sys

with codecs.open('./goodinfo.html','r',encoding='utf8') as f:
    text = f.read()


soup = BeautifulSoup(text.replace('&nbsp;', '').replace('　',''), 'lxml')
#[s.extract() for s in soup('thead')]

#print soup
df = pd.read_html(str(soup))

for index, row in df[0].iterrows():
    k = float(re.findall(r"\d+\.?\d*", row['K值(月)'])[0])
    d = float(re.findall(r"\d+\.?\d*", row['D值(月)'])[0])
    print(k)
    print(d)
    #print(row['D值(月)'])
    #print(row['K值(月)'])
    #print(type(row['K值(月)']))
    '''
    print(type(item))
    for k, d in item['K值(月)'], item['D值(月)']:
        #print(type(it))
        k_ = re.findall(r"\d+\.?\d*", k)
        d_ = re.findall(r"\d+\.?\d*", d)
        print(type(s[0]))
        #print(it)
        #print(re.findall(r"\d+\.?\d*", item['K值(月)'][0]))
    #print(item['K值(月)'][0])
    #print(item['K值(月)'][1])
    '''

