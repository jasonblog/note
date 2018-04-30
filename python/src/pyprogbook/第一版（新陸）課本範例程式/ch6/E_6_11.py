#E_6_11.py 功能: 實際參數與「**」的用法，代入映射物件的範例
def transfer(w, A,B,C):
#transfer: 傳進的原始值*w後放入字典回傳
    newdic={'甲': w*A, '乙': w*B, '丙': w*C}
    return (newdic)
weight=10    
dic={'A': 9, 'B': 8, 'C':7}
print('result =',transfer(weight, **dic))