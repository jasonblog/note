#E_7_3.py功能: 讀取每股價格乘以1000寫回另一個文字檔
with open('./file/s3008_read.txt', 'r') as fin: 
    with open('./file/s3008_write.txt', 'w') as fout:
        for line in fin:
            data=float(line)*1000
            print('每股價格:%5.0f, 每張價格:%8.0f' %(float(line), data))  
            fout.write(str(data)+'\n')   