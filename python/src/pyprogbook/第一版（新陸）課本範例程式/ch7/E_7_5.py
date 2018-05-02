#E_7_5.py功能: 文字檔的讀寫方法-readline()1次讀取一列
fin = open('./file/oil.txt', 'r')
while True:
    line=fin.readline()    
    if not line: break
    print('---',line, end='')
fin.close()