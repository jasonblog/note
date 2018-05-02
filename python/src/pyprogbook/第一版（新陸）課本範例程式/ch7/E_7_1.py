#E_7_1.py功能: 讀取指定路徑下的文字檔
fin = open('D:/ipython/ch7/DuFu_poetry1.txt','r')
for line in fin:
    print(line, end='')
fin.close()