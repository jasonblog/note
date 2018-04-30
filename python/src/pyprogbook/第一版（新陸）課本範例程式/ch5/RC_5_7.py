# RC_5_7 功能: 身分證號碼檢查。
data=input('請輸入身份證字號 = ')
id=data.upper()
a1_dic={'A': '10', 'B': '11', 'C': '12', 'D': '13', 'E': '14', 'F': '15', \
'G': '16', 'H': '17', 'J': '18', 'K': '19', 'L': '20', 'M': '21',\
'N': '22', 'P': '23', 'Q': '24', 'R': '25', 'S': '26', 'T': '27', 'U': '28',\
'V': '29', 'X': '30', 'Y': '31', 'W': '32', 'Z': '33'}
N=len(id)
if (id.isalnum()) & (N==10) & (id[0].isalpha()) & (id[1:N].isdigit()):
    A=id[0]
    a1_value=a1_dic.get(A)
    X1=int(a1_value[0])
    X2=int(a1_value[1])
    w=1
    sumn=0
    for k in range(N-2, 0, -1):
        sumn=sumn+(int(id[w])*k)
        w+=1
        print('sumn ', sumn)        
    Y=X1+(X2*9)+sumn+int(id[w])
    print('Y = ', Y)
    [quotient, remainder]=divmod(Y,10)
    if remainder==0:
        print('身份証號碼規則驗證正確')
    else:
        print('身份証號碼規則驗證「不」正確')
else: 
    print('請輸入第1個字元為英文字母，9個阿拉伯數字')   