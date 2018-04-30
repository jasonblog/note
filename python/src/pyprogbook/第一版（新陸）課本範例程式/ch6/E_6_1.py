#sumn_def.py功能: 主程式呼叫1層自訂函數
def sumnfunc(n):
#A_func功能: 計算累加
    sumn=0
    for i in range(n+1):
        sumn=sumn+i
    return(sumn)
num=int(input('輸入一個正整數n = '))    
result=sumnfunc(num)
print('1累加到n的結果 = ', result)

