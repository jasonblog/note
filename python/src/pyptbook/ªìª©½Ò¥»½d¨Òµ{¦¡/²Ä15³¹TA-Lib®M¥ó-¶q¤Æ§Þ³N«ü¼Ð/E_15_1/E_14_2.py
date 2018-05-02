
# -*- coding: utf-8 -*-
"""
Created on Thu Jun 22 09:11:04 2017
1. 自Google finance抓取台股日資料(任意期間)
2. 存入EXCEL檔案
@author: 蔡進金, 林萍珍
"""
import pandas as pd
import datetime
import time
 
#引入自建類別ProgramTrade class_GetGoogleFinance
import ProgramTrade.class_GetGoogleFinance as Gf
 
#主程式
def main(stkno,start,end):
    #產生GetGoogleFinance類別的物件實體(gf)
    gf=Gf.GetGoogleFinance(stkno,start,end,False)   
    #取出物gf物件實體的資料指派給Data
    RowData =gf.getstock(True)
    #將股價資料轉成dataframe
    df = pd.DataFrame(RowData)
    #呼叫存入excel檔的方法
    gf.savetoexcel(df,stkno)    #Save data to excel
    status_text = gf.excel_to_db(df,stkno)
    return status_text

 
#呼叫主程式並代入股票代號與起訖日期
if __name__ == "__main__":  
    starttime = time.clock()
    stkno='1102'
    start = datetime.datetime(2007,10,1)
    end = datetime.datetime(2017,9,30)
    df = main(stkno,start,end) #呼叫主程式
    endtime = time.clock()
    print('程式執行時間 = %d%s' %(round(endtime-starttime), '秒'))
