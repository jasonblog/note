# E_8_9: 計累加的程式，加入異常處理機制
def sumn(n):
    sn=0
    if n<=0: 
        raise ValueError('輸入的參數必須大於零哦')
    else: 
        for i in range(1, n+1):
            sn=sn+i
        return(sn)
while True: 
    try: 
        num=int(input('請輸入正整數: '))
        result=sumn(num)
    except ValueError as v:
        print('數值錯誤，' + str(v))            
    except:
        print('未知錯誤')        
    else: 
        print('1加到%d = %d' %(num, result))
        break