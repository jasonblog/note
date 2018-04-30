# E_5_15 功能: 積分計算面積。
dx=1
area=0
y=0
for x in range(200,500,dx):
    x=x/100
    y=(x**2)+1
    area=area+(y*(dx/100))
print('area = ', area)  