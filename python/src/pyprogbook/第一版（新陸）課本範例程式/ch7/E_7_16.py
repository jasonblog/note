#E_7_16: 上網路抓取多家股價資料
import pandas as pd
import pandas.io.data as web
import datetime
import time
starttime = time.clock()
#下載資料起始日與股票代號
start = datetime.datetime(2000,1,1)
end = datetime.datetime(2016,4,19)
writer=pd.ExcelWriter('./file/stocprice.xlsx')
#以迴圈依公司代號順序抓取資料，並個別依公司代號命名工作表
stockid=('2303', '2330', '3008', '2498', '2311', '2409', '2357', '2317')
print(type(stockid))
for i in range(0,len(stockid)):
    sid=stockid[i]+'.tw'   
    df = web.DataReader(sid,'yahoo',start,end)
    df.to_excel(writer,stockid[i])
#日股價資料寫入excel檔
writer.save()
endtime = time.clock()
print('程式執行時間 = %d %s' %(round(endtime - starttime), '秒'))