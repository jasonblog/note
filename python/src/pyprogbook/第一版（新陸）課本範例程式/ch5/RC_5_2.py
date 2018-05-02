# RC_5_2 功能: 景氣對策信號。
score=int(input('請輸入景氣對策信號綜合分數: '))
if score>38:
    print('紅燈')
elif score>32:
    print('黃紅燈')
elif score>23:
    print('綠燈')
elif score>17:
    print('黃藍燈')
else:
    print('藍燈')