# E_5_10 功能: 以巢狀迴圈產生9*9乘法表
for i in range(1, 10):  #外圈
    for j in range(1, 10):  #內圈
        print('%d*%d=%2d  '%(i,j,i*j), end='')
    print('\n')