# E_5_15_2 功能: 積分計算面積(1)。
import numpy as np
dx=float(input('Please input dx =' ))
area=0
y=0
for x in np.arange(2, 5, dx):
    y=(x**2)+1
    area=area+(y*dx)
print('area = ', area)