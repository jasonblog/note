#RC_7_2: 大盤指數每分鐘資料82萬筆
import pandas as pd
import time
starttime = time.clock()
df = pd.read_csv('./file/TXF2003.txt', sep=",")
df.columns = ['Date', 'Time', 'Open', 'High', 'Low','Close', 'Vol']
print(df.describe())
endtime = time.clock()
print('程式執行時間 = %d %s' %(round(endtime - starttime), '秒'))