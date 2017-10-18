# ultrasound raw data

- test.py

```py
#!/usr/bin/python
# -*- coding: utf-8 -*-
import matplotlib.pyplot as plt
import numpy as np
import re
import sys

def get_nodes(inputline):
    resultx=[]
    resulty=[]
    resultz=[]

    p = re.compile("u\,\d+\,\d+\,\d+,\d+,([-+]?[0-9]*\.?[0-9]+)\,\d+\,([-+]?[0-9]*\.?[0-9]+)\,\d+\,([-+]?[0-9]*\.?[0-9]+)\,\d+\,\d+\,\d+\,([-+]?[0-9]*\.?[0-9]+)\,\d+\,([-+]?[0-9]*\.?[0-9]+)\,\d+\,([-+]?[0-9]*\.?[0-9]+)\,\d+\,\d+\,\d+\,([-+]?[0-9]*\.?[0-9]+)\,\d+\,([-+]?[0-9]*\.?[0-9]+)\,\d+\,([-+]?[0-9]*\.?[0-9]+)")
    lines = inputline.read()
    lines = lines.strip().split('\n')
    lens = len(lines)
    for i in range(0, lens, 1):
        resultx.append(p.findall(lines[i])[0][0])
        resulty.append(p.findall(lines[i])[0][1])
        resultz.append(p.findall(lines[i])[0][2])
	#print p.findall(lines[i])[0]
	#print type(p.findall(lines[i])[0][0])

    plt.rcParams.update({'font.size': 9})
    plt.figure(1)
    plt.subplot(311)   
    plt.title('pitch ' + ' max=' + str(max(resultx)).strip() + ", min=" + str(min(resultx)).strip() + ', Peak-Peak=' + str(abs(float(max(resultx)) - float(min(resultx)))).strip()) 
    plt.plot(resultx)
    plt.subplot(312)   
    plt.title('yaw ' + ' max=' + str(max(resulty)).strip() + ", min=" + str(min(resulty)).strip() + ', Peak-Peak=' + str(abs(float(max(resulty)) - float(min(resulty)))).strip()) 
    plt.plot(resulty)
    plt.subplot(313)   
    plt.title('roll ' + ' max=' + str(max(resultz)).strip() + ", min=" + str(min(resultz)).strip() + ', Peak-Peak=' + str(abs(float(max(resultz)) - float(min(resultz)))).strip()) 
    plt.plot(resultz)
    plt.show()

'''

    resultx=[]
    resulty=[]
    resultz=[]
    for x in lines:
	resultx.append(x.split(' ')[0])
	resulty.append(x.split(' ')[1])
	resultz.append(x.split(' ')[2])

    plt.rcParams.update({'font.size': 9})
    plt.figure(1)
    plt.subplot(311)   
    plt.title('pitch ' + ' max=' + str(max(resultx)).strip() + ", min=" + str(min(resultx)).strip() + ', Peak-Peak=' + str(abs(float(max(resultx)) - float(min(resultx)))).strip()) 
    plt.plot(resultx)
    plt.subplot(312)   
    plt.title('yaw ' + ' max=' + str(max(resulty)).strip() + ", min=" + str(min(resulty)).strip() + ', Peak-Peak=' + str(abs(float(max(resulty)) - float(min(resulty)))).strip()) 
    plt.plot(resulty)
    plt.subplot(313)   
    plt.title('roll ' + ' max=' + str(max(resultz)).strip() + ", min=" + str(min(resultz)).strip() + ', Peak-Peak=' + str(abs(float(max(resultz)) - float(min(resultz)))).strip()) 
    plt.plot(resultz)
    plt.show()
    '''

if __name__ == "__main__":
    fin = open(sys.argv[1])
    get_nodes(fin)
```

- data.txt

```sh
u,19145655,25,1,3,232.91,3106,253.54,2748,225.71,3166,5,3,266.24,8660,300.60,10212,301.33,10256,9,3,246.03,8847,241.88,9098,284.36,9221
u,19146488,25,1,3,233.28,3140,254.09,2740,226.01,3196,5,3,266.48,8677,300.42,10181,301.70,10357,9,3,246.15,8856,241.46,8992,282.90,9128
u,19147321,25,1,3,233.58,3120,253.72,2716,224.73,3206,5,3,266.30,8658,299.87,10096,301.82,10263,9,3,246.09,8780,242.31,9140,283.94,9138
u,19148154,25,1,3,233.34,3230,254.03,2737,226.50,3221,5,3,265.99,8638,299.93,10153,301.39,10250,9,3,246.15,8778,242.00,9042,283.81,9155
u,19148987,25,1,3,233.89,3251,254.88,2751,225.28,3172,5,3,266.17,8623,299.99,10152,301.70,10252,9,3,245.91,8715,242.19,9085,284.12,9106
u,19149820,25,1,3,233.03,3187,253.78,2721,226.75,3194,5,3,266.48,8597,300.84,10320,301.57,10177,9,3,245.79,8664,241.64,8937,284.18,9136
u,19150652,25,1,3,233.22,3136,253.36,2719,225.22,3173,5,3,266.42,8559,301.03,10321,301.51,10085,9,3,245.97,8674,241.52,9013,284.24,9171
u,19151485,25,1,3,234.25,3162,255.00,2839,228.58,3490,5,3,266.11,8481,300.60,10325,301.94,10162,9,3,246.09,8728,242.00,8912,283.75,9114
u,19152318,25,1,3,232.91,3105,253.91,2710,225.71,3226,5,3,266.42,8494,300.60,10357,302.25,10210,9,3,246.34,8780,241.82,8869,282.59,8995
u,19153151,25,1,3,233.15,3165,253.36,2715,225.46,3172,5,3,266.05,8411,300.54,10360,301.76,10135,9,3,245.67,8774,242.61,9033,284.06,9215
u,19153983,25,1,3,233.34,3186,254.33,2746,225.95,3256,5,3,266.30,8418,300.90,10432,301.45,10097,9,3,246.09,8834,242.07,8971,283.81,9092
```

```sh
python test.py data.txt
```