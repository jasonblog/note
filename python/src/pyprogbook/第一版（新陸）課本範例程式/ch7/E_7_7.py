#E_7_7.py功能: readlines()配合for每次取出串列並忽略表頭
with open('./file/oil.txt', 'r') as fin:
    with open ('oil_write.txt', 'w') as fout:
        for line in fin.readlines():    
            if line.startswith('日'):  #判断表頭的第1個字  
                continue 
            print(line, end='')
            fout.write(line)
