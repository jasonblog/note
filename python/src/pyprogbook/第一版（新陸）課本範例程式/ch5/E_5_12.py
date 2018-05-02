# E_5_12 功能:以while迴圈產生9*9乘法表結合break 與continue。
i=1
j=1
while i<=9: #外圈
    if i==4:
        i+=1
        continue
    while j<=9:  #內圈
        if j==7:
            break
        print('%d*%d=%2d  '%(i,j,i*j), end='')
        j+=1
    j=1
    i+=1
    print('\n')    