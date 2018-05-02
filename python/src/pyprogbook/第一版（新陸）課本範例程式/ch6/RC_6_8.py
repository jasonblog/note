#RC_6_8 功能: 年金給付金額
def annuity(pv, i, n):
#annuity: 計算年金公式 
    up=i*((1+i)**n)
    down=((1+i)**n)-1
    result=pv*(up/down)
    return(result)
i=0.03
n=int(input('輸入貸款年數 = '))
pv=1000000
print('每年應付貸款金額 = %10.2f' %(annuity(pv, i,n)))