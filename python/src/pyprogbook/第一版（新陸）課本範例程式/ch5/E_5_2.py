#E_5_2 功能: 輸入一個數值，判斷若小於50則開根號乘以10，否則加10。
import math
num=int(input('請輸入數據1-100: '))
if num<50:
    total=math.sqrt(num)*10
else: 
    total=num+10    
print('total=%d' % round(total))