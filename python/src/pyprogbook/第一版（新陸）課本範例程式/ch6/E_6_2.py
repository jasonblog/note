#E_6_2.py功能: 主程式呼叫2層的自訂函數
def callfunc(n):
#callfunc功能: 檢查數值型別與呼叫函數
    if n.isdigit():
        number=int(n)        
        print('1累加到n的結果 = ', sumnfunc(number))        
        print('1累乘到n的結果 = ', prodnfunc(number))        
    else:
        print('請輸入數值資料')    
def sumnfunc(n):
#sumnfunc: 計算累加    
    sumn=0
    for i in range(1,n+1):
        sumn=sumn+i
    return(sumn)   
def prodnfunc(n):
#prodnfunc功能: 計算累乘
    prodn=1
    for i in range(1,n+1):
        prodn=prodn*i
    return(prodn)    
num=input('輸入一個正整數n = ')
callfunc(num)


