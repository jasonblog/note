#RC_6_6 功能: 現值
def pvfix(fv, i, n):
#fvfix: 計算現值公式    
    result=fv/((1+i)**(n))
    return(result)
fv=float(input('輸入終值 = '))    
i=0.03
n=int(input('輸入n年前 = '))
print('%d年前的現值 = %6.2f' %(n, pvfix(fv, i, n)))
