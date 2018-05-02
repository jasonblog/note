#RC_6_2 功能: 水族箱中的5到30個月後魚群數目估計
import math
def fishev(t):
#fishev: 計算魚群數目    
    expv=25*math.exp(-0.3*t)
    return(1000/(1+expv))
for t in range(5, 30+5,5):
    result=round(fishev(t),2)
    print('%2d個月後的魚群數目 = %10.2f' %(t, result))
    
