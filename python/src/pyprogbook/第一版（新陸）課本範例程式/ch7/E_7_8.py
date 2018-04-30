#E_7_8.py功能: 取得檔案物件的其他屬性
with open('./file/相思王維.txt', 'r', encoding='UTF-8') as fin:
    print('Name of file: ', fin.name)
    print('Closed or not: ', fin.closed)
    print('Opening mode: ', fin.mode)
    for line in fin.readlines(): 
        print(line, end='')