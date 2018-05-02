#E_6_10.py 功能: 實際參數加*號，代入可迭代者的範例
def func1(x, coe1,coe2):
#func1: 計算x乘上系數後再相加
    return (x*coe1)+(x*coe2)
print('result =',func1(3, *range(1,3)))