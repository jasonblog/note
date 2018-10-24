#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import pandas as pd
from bs4 import BeautifulSoup
import codecs
import sys

with codecs.open('./goodinfo.html','r',encoding='utf8') as f:
    text = f.read()


soup = BeautifulSoup(text.replace('&nbsp;', '').replace('　',''), 'lxml')
[s.extract() for s in soup('thead')]

#print soup
df = pd.read_html(str(soup))
print(df)

