# RC_5_2 功能: 景氣對策信號。
score=int(input('請輸入景氣對策信號綜合分數 = '))
if score>=45 or score<9:
    print('錯誤')
elif score>=37:    
    print('紅燈')
elif score>=31:
    print('黃紅燈')
elif score>=22:
    print('綠燈')
elif score>=16:
    print('黃藍燈')
elif score>=9:
    print('藍燈')