#E_5_1 功能: 輸入一個數值，判斷若小於50則開根號乘以10
import math
num=int(input('請輸入任一數'))
Tt=num
if num<50:
    Tt=math.sqrt(num)*10
print(num,Tt)
