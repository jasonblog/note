#E_6_12.py 功能: 形式參數與「*」的用法，代入空的tuple範例
def sumdata(a,b,c,*d):
#sumdata: 傳進的原始值加總, tuple也加總
    sum1=a+b+c
    sum2=sum(d)
    return (sum1, sum2)
[t1, t2]=sumdata(5,6,8,2,6,8)
print('total1 = %d, total2 = %d, max = %d' %(t1, t2, max(t1,t2)))
