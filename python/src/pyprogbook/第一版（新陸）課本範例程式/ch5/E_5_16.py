# E_5_16 功能: 排列組合。
m=int(input('輸入 m= '))
n=int(input('輸入 n= '))
prodi=1 #計算m!
prodj=1 #計算n!
prodk=1 #計算m-n!
for i in range(m,1,-1):
    prodi=prodi*i
for j in range(n,1,-1):
    prodj=prodj*j  
k=m-n    
for k in range(k,1,-1):
    prodk=prodk*k
number=prodi/(prodj*prodk)
print('%s %d %s' %('排列組合共有', number, '方法')) 