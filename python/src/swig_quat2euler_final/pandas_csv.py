# -*- coding: utf-8 -*-
import pandas as pd
from TestModel import *
f = open('example.csv', 'r')
df = pd.read_csv(f)
lens = len(df)

for i in range(lens):
    #print df['Timestamp'][i]
    '''
    print df['quat[0]'][i],
    print df['quat[1]'][i],
    print df['quat[2]'][i],
    print df['quat[3]'][i]
    '''
    Quaternion_Quat2Euler(df['quat[0]'][i], df['quat[1]'][i], df['quat[2]'][i], df['quat[3]'][i], xzy)

