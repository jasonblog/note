#E_5_8 功能: 計算1-(1/n)的函數。
n=int(input('Please input n : '))
result=0
for i in range(2,n+1,1):
    result=result+(1-(1/i))
print('%s %.2f' %('The result is ', result))