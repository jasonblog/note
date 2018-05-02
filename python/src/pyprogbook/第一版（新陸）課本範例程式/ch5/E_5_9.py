# E_5_9 功能: 以雙迴圈(9列，9行)產生亂數(0到9)模擬二維陣列
import random as rd
for i in range(1, 10):  #外圈
    for j in range(1, 10):  #內圈
        num=rd.randint(0,9)  
        print('%3d'%(num), end='')
    print('\n')