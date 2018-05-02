#RC_8_2: 計算有率利率加入異常處理
def earfix(rn, m):
    #earfix: 計算有效利率公式   
    if m<=0 | m>365: 
        raise ValueError('輸入的參數必須大於零哦')
    elif type(m) is not int:
        raise TypeError('輸入的參數必須是整數哦')
    else: 
        ear=((1+(rn/m))**m)-1
    return(ear)  
#設定年、半年、季、月、週、日複利的次數    
m=[1, 2, 4, 12, 52, 365]
rn=0.06
for i in m:
    try: 
        ear=earfix(rn, i)
    except ValueError as v:
        print('數值錯誤，' + str(v))            
        break         
    except TypeError as t :
        print('型別錯誤，' + str(t))  
        break         
    else: 
        print('年複利次數%4d, 有效利率 = %6.4f%%' %(i, ear*100))