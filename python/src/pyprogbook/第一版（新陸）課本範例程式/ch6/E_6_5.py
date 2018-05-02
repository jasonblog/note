#E_6_5.py 功能: 傳值的操作範例
def callbyvalue(number):
#callbyvalue函數功能: 計算參數乘上自己後回傳    
    number*=number
    return number
sample=4
print('呼叫函數前的原始值', sample)
print('呼叫函數後的內容值', callbyvalue(sample))
print('呼叫函數前的原始值不會被改變', sample)


