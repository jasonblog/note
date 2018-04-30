#RC_6_4 功能: 終值
def fvfix(pv, i, n):
#fvfix: 計算終值公式    
    result=pv*(1+i)**n
    return(result)
pv=100
i=0.03
n=int(input('計算終值的年數 = '))
print('%d年後的終值 = %6.2f' %(n, fvfix(pv, i, n)))