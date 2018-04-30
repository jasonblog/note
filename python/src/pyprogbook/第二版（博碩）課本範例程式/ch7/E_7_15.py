#E_7_15: 上網路抓取股價資料
import pandas as pd
import pandas_datareader.data as web
import datetime
#下載資料起始日與股票代號
start = datetime.datetime(2016,4,17)
end = datetime.datetime(2017,5,16)
df = web.DataReader('AAPL','yahoo',start,end)
#日股價資料寫入excel檔
writer=pd.ExcelWriter('./file/stock2330_2017.xlsx')
df.to_excel(writer,'Sheet1')
writer.save()