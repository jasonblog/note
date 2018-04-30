#RC_6_5 功能: 計算退休金
def FVOA(PMT, i, n):
#FVOA: 計算普通年金終值公式    
    FVIFA=0
    #計算年金終值複利因子
    for t in range(1, n+1):
        FVIFA=FVIFA+((1+i)**(n-t))
    print('FVIFA =', round(FVIFA,2))        
    #計算退休金=年金乘上複利因子        
    fv=PMT*FVIFA
    return(fv)
PMT=int(input('輸入每個月存入金額 = '))    
i=0.025
n=int(input('輸入預計幾年後退休 = '))
print('%d年後的終值 = %10.2f' %(n, FVOA(PMT, i/12,n*12 )))