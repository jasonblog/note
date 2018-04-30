#E_7_14: 建立、更改、取得、刪除檔案路徑
import os
import os.path
if os.path.exists('./newdir'):
    os.rmdir('./newdir')
    print(os.getcwd())    
else: 
    os.mkdir('./newdir')  
    os.chdir('./newdir')
    print(os.getcwd())    