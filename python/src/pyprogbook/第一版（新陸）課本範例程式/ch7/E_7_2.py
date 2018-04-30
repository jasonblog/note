#E_7_2.py功能: 以with as讀取文字檔
with open('./file/DuFu_poetry1.txt','r') as fin: 
    for line in fin:
        print(line, end='')

