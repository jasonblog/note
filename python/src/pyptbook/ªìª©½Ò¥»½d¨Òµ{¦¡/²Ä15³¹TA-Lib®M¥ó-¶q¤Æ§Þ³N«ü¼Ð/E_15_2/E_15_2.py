import pymysql.cursors
import pandas as pd
import numpy as np
import datetime
import time
#引用talib相關套件
import talib
from talib import MA_Type

def SMAWMA(df, count, acmroi, winnum, winfact):
    close = np.array(df['close'], dtype=float)
    SMA = talib.SMA(close,5) #close 代進SMA方法做計算
    WMA = talib.WMA(close,5) #close 代進WMA方法做計算
    df['SMA'] = SMA
    df['WMA'] = WMA
#    print(df)
    #設定初始值
    df['XBuy'] = np.nan
    df['YBuy'] = np.nan
    df['XSell'] = np.nan
    df['YSell'] = np.nan
    row = len(df)
    flag = False
    change = 0
    buyprice=[]
    sellprice=[] 
    win = 0
    loss = 0
    roi = 0
    for i in range(row):
        change = df['WMA'].iloc[i]/ df['SMA'].iloc[i]
        if (flag == False) & (df['WMA'].iloc[i] > df['SMA'].iloc[i]) & (change >= 1.02):
            df['XBuy'].iloc[i] =  df['dates'].iloc[i]
            df['YBuy'].iloc[i] = df['close'].iloc[i]
            buyprice = df['close'].iloc[i]
            flag = True
        if (flag == True) & (df['WMA'].iloc[i] <= df['SMA'].iloc[i]) & (change <= 0.98):
            df.XSell[i] = df['dates'].iloc[i]
            df.YSell[i] = df['close'].iloc[i]
            sellprice = df['close'].iloc[i]
            count += 1
            flag = False 
            [roi, winnum] = roical(buyprice, sellprice, winnum)
            acmroi += roi
            [loss, win] = winfactor(buyprice, sellprice, loss, win)
        if (flag == True & i==(row-1)):
            df.XSell[i] = df['dates'].iloc[i]
            df.YSell[i] = df['close'].iloc[i]
            sellprice = df['close'].iloc[i]
            count += 1
            [roi, winnum] = roical(buyprice, sellprice, winnum)
            acmroi += roi
    if (loss == 0):
        loss = 1
    winvar = win / loss
    print(' win = ', win)
    print('loss = ', loss)
    print('winvar = ', winvar)
    if (count == 0):
        count = 0.01  
    str1 = 'SMAWMA策略: ' +'交易次數 = '+ str(count) + ' 次; ' + '累計報酬率 = ' + str(round(acmroi*100, 2)) + '%;' + '勝率 = ' + str(round((winnum/count)*100,2)) + '%' + '; 獲利因子 = ' + str(round(winvar, 2)) 
    print(str1)          
    return (count, acmroi, winnum, winvar) 
#BbandMA自訂函數
def BBandMA(df, count, acmroi, winnum, winfact):
    #設定初始值
    df['BBXBuy'] = np.nan
    df['BBYBuy'] = np.nan
    df['BBXSell'] = np.nan
    df['BBYSell'] = np.nan
    #計算BBand上, 中, 下線
    close = np.array(df['close'], dtype=float)
    upper, middle, lower = talib.BBANDS(close, timeperiod=20, nbdevup=0.0001, nbdevdn=0.0001, matype=MA_Type.T3)
    df['BBupper'] = upper
    df['BBlower'] = lower
    vol = df['volume'].as_matrix().astype("float64") 
    df['volMA'] = talib.SMA(vol, timeperiod=20)   
    flag = False
    buyprice=[]
    sellprice=[] 
    win = 0
    loss = 0
    roi = 0
    for i in range(len(df)):
        if (flag == False) & (float(df['high'].iloc[i]) > float(df['BBupper'].iloc[i])) & (float(df['volume'].iloc[i]) > (float(df['volMA'].iloc[i]) * 2)) :
            buyprice = df['close'].iloc[i]
            df['BBXBuy'].iloc[i] =  df['dates'].iloc[i]
            df['BBYBuy'].iloc[i] = buyprice
            flag = True
        if (flag == True) & (float(df['low'].iloc[i]) < float(df['BBlower'].iloc[i])) & (float(df['volume'].iloc[i]) > (float(df['volMA'].iloc[i]) * 2)):
            sellprice = df['close'].iloc[i]
            df.BBXSell[i] = df['dates'].iloc[i]
            df.BBYSell[i] = sellprice
            count += 1
            flag = False 
            [roi, winnum] = roical(buyprice, sellprice, winnum)
            acmroi += roi
            [loss, win] = winfactor(buyprice, sellprice, loss, win)
        if (flag == True & i==(len(df)-1)):
            df.BBXSell[i] = df['dates'].iloc[i]
            sellprice = df['close'].iloc[i]
            df.BBYSell[i] = sellprice
            count += 1
            [roi, winnum] = roical(buyprice, sellprice, winnum)
            acmroi += roi
    if (loss == 0):
        loss = 1
    winvar = win / loss
    if (count == 0):
        count = 0.01      
    str1 = 'BBand策略: ' + '交易次數 = '+ str(count) + ' 次; ' + '累計報酬率 = ' + str(round(acmroi*100, 2)) + '%; ' + '勝率 = ' + str(round((winnum/count)*100,2)) + '%' + '; 獲利因子 = ' + str(round(winvar, 2)) 
    print(str1)
    return (count, acmroi, winnum, winvar) 
#計算報酬率與勝率
def roical(buyprice, sellprice, winnum):
    roi = ((float(sellprice) - float(buyprice)) / float(buyprice))
    if roi > 0:
        winnum+=1
    return (roi, winnum)
#計算獲利因子要用的累計獲利金額與損失金額
def winfactor(buyprice, sellprice, loss, win):
    payoff = float(buyprice) - float(sellprice)
    if payoff > 0:
        loss += payoff
    else:
        win += (-payoff)
    return (loss, win)
#查詢資料庫的股票資料
def select_db(stkno):
    connection = pymysql.connect(host = 'localhost',
                         user = 'root',
                         password = 'fish9850',
                         db = 'mydatabase',
                         charset = 'utf8mb4')
    try:
        with connection.cursor() as cursor:

            #取股票名
            table_name = 'stock_'+str(stkno)
            table_select_sql = "SELECT * FROM `%s`" % (table_name)
            df = pd.read_sql(table_select_sql, connection)
            return df

    except Exception as e:
        print("Exeception occured:{}".format(e))

    finally:
        connection.close()
#主程式
def main(stkno, dfSMA, dfBBand,i): 
    df = select_db(stkno)     
    #呼叫SMAWMA策略
    [count, acmroi, winnum, winfact] = SMAWMA(df, count = 0, acmroi = 0, winnum = 0, winfact = 0)
    if (count == 0):
        count = 0.01  
    dfSMA['count'].iloc[i] = count
    dfSMA['roi'].iloc[i] = round(acmroi * 100,2)
    dfSMA['winrate'].iloc[i] = round((winnum / count) * 100,2)
    dfSMA['winfactor'].iloc[i] = winfact     
    print('dfSMA = ', dfSMA)
    #呼叫BBand策略
    [count, acmroi, winnum, winfact] = BBandMA(df, count = 0, acmroi = 0, winnum = 0, winfact = 0)  
    if (count == 0):
        count = 0.01        
    dfBBand['count'].iloc[i] = count
    dfBBand['roi'].iloc[i] = round(acmroi * 100,2)
    dfBBand['winrate'].iloc[i] = round((winnum / count) * 100,2)
    dfBBand['winfactor'].iloc[i] = winfact  
    print('dfBBand = ', dfBBand)
    return (dfBBand.to_html(classes=''))
#呼叫主程式並代入股票代號與起訖日期
if __name__ == "__main__":   
    starttime = time.clock()
    dfSMA = pd.read_excel('result_SMA.xlsx')
    dfBBand = pd.read_excel('result_BBand.xlsx')    
    writer= pd.ExcelWriter('FinalResult.xlsx')
    for i in range(len(dfSMA)):
        print(dfSMA['id'].iloc[i])
        stkno=dfSMA['id'].iloc[i]
        start = datetime.datetime(2007,10,1)
        end = datetime.datetime(2017,9,30)
        main(stkno,start,end, dfSMA, dfBBand, i)
    print('dfSMA = ', dfSMA)
    print('dfBBand = ', dfBBand)
    dfSMA.to_excel(writer, 'SMA')
    dfBBand.to_excel(writer, 'BBand')
    writer.save()
    endtime = time.clock()
    print('程式執行時間 = %d%s' %(round(endtime-starttime), '秒'))