#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import pandas as pd
from bs4 import BeautifulSoup
import codecs
import re
import sys

df =  pd.DataFrame({'A':[23,25,10], 'B':[7,8,3], 'C':[8,3,1]})
print (df)

def highlight_col(x):
    #copy df to new - original data are not changed
    df = x.copy()
    #set by condition
    mask = df['A'] == 23
    df.loc[mask, :] = 'background-color: yellow'
    df.loc[~mask,:] = 'background-color: ""'
    return df

df = df.style.apply(highlight_col, axis=None)
print(type(df))
