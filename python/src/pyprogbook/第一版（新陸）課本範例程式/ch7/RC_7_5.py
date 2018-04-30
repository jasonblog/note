#RC_7_5: 計算技術指標-相對強弱指標RSI
import pandas as pd
import pandas.io.data as web
import numpy as np
import datetime
#步驟2:下載資料起始日與股票代號
start = datetime.datetime(2013,1,1)
end = datetime.datetime(2015,12,31)
df1 = web.DataReader('2379.tw','yahoo',start,end)
rows=len(df1.index) #已扣掉表頭
#步驟3:計算開盤價的報酬率  
roi=df1[df1.columns[0]].pct_change()
#步驟5:計算RSI
def rsi(n, rows, roi):
    rsiv=[]
    rsiv[0:n]=np.linspace(0,0, n+1)
    for i in range (n, roi.count()):
        ups=0
        downs=0
        u=0
        d=0
        ud=0
        for j in range(i-n+1,i+1):
            if roi[j]>=0.00000000:        
                u=u+roi[j]
            else: 
                d=d+roi[j]                    
        ups=u/n
        downs=d/n            
        ud=ups/downs
        rsiv.append(abs(100-(100/(1+ud))))
    return rsiv
#步驟4:依天期設定進迴圈呼叫rsi函數 
for n in range(5, 11, 5):
    rsi_data=[]
    rsi_data=rsi(n, rows, roi)#呼叫rsi自訂函數
    #將rsi資料插入到df1
    colname='rsi'+str(n)
    df1[colname] = rsi_data
print(df1)
#步驟6:寫入excel檔
writer=pd.ExcelWriter('./file/2379.xlsx')
df1.to_excel(writer,'2379')
workbook = writer.book
worksheet = writer.sheets['2379']