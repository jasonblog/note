#!/usr/bin/python
# -*- coding: utf-8 -*-

# python pandas_csv.py zyx example.csv

import pandas as pd
import sys
from TestModel import *

RotSeq = {'zyx' : zyx, 'zyz' : zyz, 'zxy' : zxy, 'zxz' : zxz, 'yxz' : yxz, 'yxy' : yxy, 'yzx' : yzx, 'yzy' : yzy, 'xyz' : xyz, 'xyx': xyx, 'xzy' : xzy, 'xzx' : xzx}

def main(argv):
    rotSeq = RotSeq['xyz']
    filename = 'example.csv'

    if len(argv) == 2:
        rotSeq = RotSeq[argv[0]]
        filename = argv[1]

    f = open(filename, 'r')
    df = pd.read_csv(f)
    lens = len(df)

    for i in range(lens):
        '''
        print df['quat[0]'][i],
        print df['quat[1]'][i],
        print df['quat[2]'][i],
        print df['quat[3]'][i]
        '''
        Quaternion_Quat2Euler(df['quat[0]'][i], df['quat[1]'][i], df['quat[2]'][i], df['quat[3]'][i], rotSeq)

if __name__ == "__main__":
    main(sys.argv[1:])
