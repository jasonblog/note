#E_7_13: 檔案更名與檔案刪除
import os
import os.path
PATH1='./file/WangWei_poetry_1.txt'
PATH2='./file/WangWei_poetry_2.txt'
if os.path.isfile(PATH1) and os.access(PATH1, os.R_OK):
    os.rename('./file/WangWei_poetry_1.txt', '辛夷塢王維.txt')   
    print('%s檔案已更名%s' %('WangWei_poetry_1.txt','辛夷塢王維.txt'))
else:
    print('WangWei_poetry_1.txt檔案不存在')    
if os.path.isfile(PATH2) and os.access(PATH2, os.R_OK): 
    os.remove(PATH2)
    print('%s檔案已刪除' %('WangWei_poetry_2.txt'))    
else:
    print('WangWei_poetry_2.txt檔案不存在')
