#E_6_4.py 功能: 傳址的操作範例
def callbyreference(number):
#callbyreference函數功能: 計算平均新增到傳入參數之後    
    n=len(number)
    meanv=sum(number)/n
    number.append(meanv)
    return number
sample=[5,8,9,6,4,1,5,3,6,2]
print('呼叫函數前的原始值', sample)
print('呼叫函數後的內容值', callbyreference(sample))
print('呼叫函數後的原始值也會被改變', sample)
