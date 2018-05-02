# E_5_11 功能:重覆產生亂數(0至9的整數)直到產生的值是零時結束。
import random as rd
num=rd.randint(0,9) 
count=1
while num!=0:
    print(num)
    num=rd.randint(0,9) 
    count+=1
print(num)
print('%s%d%s' %('共產生亂數',count,'次'))   