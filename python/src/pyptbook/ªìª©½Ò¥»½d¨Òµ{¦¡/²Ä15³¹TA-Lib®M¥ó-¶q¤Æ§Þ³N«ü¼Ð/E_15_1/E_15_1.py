import pymysql.cursors
import pandas as pd
import numpy as np
import datetime
import time
 
#引用talib相關套件
import talib
#引用plotly相關套件
import plotly
from plotly.graph_objs import Figure
import plotly.offline as pyo
#設定plotly帳號與apikey
plotly.tools.set_credentials_file(username=' ', api_key=' ')
pyo.offline.init_notebook_mode()
 
#繪製K線圖
def showMAplot(df):
 
    #設定X,Y軸以及格式化線圖
    trace1 = {'type':'scatter',
            'x' : df['dates'],
            'y' : df['close'],
            'name':'收盤價',
            'mode':'lines'}
    trace2 = {'type':'scatter',
            'x' : df['dates'],
            'y' : df['SMA'],
            'name':'SMA',
            'mode':'lines'}
    trace3 = {'type':'scatter',
            'x' : df['dates'],
            'y' : df['WMA'],
            'name':'WMA',           
            'mode':'lines'}
    trace4 = {'type':'scatter',
            'x' : df['XBuy'],
            'y' : df['YBuy'],
            'name':'買進點',           
            'mode': 'markers',      
            'marker':dict(          
            size = 10,
            color='red' )}      
                    
    trace5 = {'type':'scatter',
            'x' : df['XSell'],
            'y' : df['YSell'],
            'name':'賣出點',           
            'mode': 'markers',
            'marker':dict(          
            symbol='diamond',   
            size = 10,  
            color='black')} 
                        
    layout = {'title':'簡單移動平均(SMA)與加權移動平均(WMA)交叉',
            'xaxis':{'title':'日期'},
            'yaxis':{'title':'價格'},
            }
 
    data = [trace1, trace2, trace3, trace4, trace5]
    fig = Figure(data=data, layout=layout)
    pyo.plot(fig) 
 
def SMAWMA(df, count, acmroi, winrate, winfact):
    close = np.array(df['close'], dtype=float)
    SMA = talib.SMA(close,5) #close 代進SMA方法做計算
    WMA = talib.WMA(close,5) #close 代進WMA方法做計算
    df['SMA'] = SMA
    df['WMA'] = WMA
    #設定初始值
    df['XBuy'] = np.nan
    df['YBuy'] = np.nan
    df['XSell'] = np.nan
    df['YSell'] = np.nan
    print(df)
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
            print(i)
            print('漲跌幅 = ',  change*100)
            print('Buy')
            print(df.close[i])
            df['XBuy'].iloc[i] =  df['dates'].iloc[i]
            df['YBuy'].iloc[i] = df['close'].iloc[i]
            buyprice = df['close'].iloc[i]
            flag = True
        if (flag == True) & (df['WMA'].iloc[i] <= df['SMA'].iloc[i]) & (change <= 0.98):
            print(i)
            print('漲跌幅 = ',  change*100)    
            print('Sell')
            print(df['close'].iloc[i])
            df.XSell[i] = df['dates'].iloc[i]
            df.YSell[i] = df['close'].iloc[i]
            sellprice = df['close'].iloc[i]
            count += 1
            flag = False
            [roi, winrate] = roical(buyprice, sellprice, winrate)
            acmroi += roi
            [loss, win] = winfactor(buyprice, sellprice, loss, win)
        if (flag == True & i==(row-1)):
            df.XSell[i] = df['dates'].iloc[i]
            df.YSell[i] = df['close'].iloc[i]
            sellprice = df['close'].iloc[i]
            count += 1
            [roi, winrate] = roical(buyprice, sellprice, winrate)
            acmroi += roi
    if loss == 0:
        loss = win
    winvar = win / loss
    return (df, count, acmroi, winrate, winvar)
#計算報酬率與勝率
def roical(buyprice, sellprice, winrate):
    roi = ((float(sellprice) - float(buyprice)) / float(buyprice))
    if roi > 0:
        winrate+=1
    return (roi, winrate)
#計算獲利因子要用的累計獲利金額與損失金額
def winfactor(buyprice, sellprice, loss, win):
    payoff = float(sellprice) - float(buyprice)
    if payoff > 0:
        win += payoff
    else:
        loss += abs(payoff)
    return (loss, win)
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
def main(stkno): 
    #查詢資料庫的股票資料
    df = select_db(stkno)           
    #初始值設定
    str1 = ''
    count = 0
    acmroi = 0
    winrate = 0
    winfact = 0
    #呼叫SMAWMA策略
    [df, count, acmroi, winrate, winfact] = SMAWMA(df, count, acmroi, winrate, winfact)
    str1 = '交易次數 = '+ str(count) + ' 次; ' + '累計報酬率 = ' + str(round(acmroi*100, 2)) + '%; .' + '勝率 = ' + str(winrate) + '; 獲利因子 = ' + str(winfact)
    print(str1)
    #將呼叫畫圖函數showMAplot
    showMAplot(df)
    #將顯示結果回傳回去
    print(datetime.datetime(2007,10,1))
    return (str1 , df.to_html(classes=''))
#    showcandle(Data)
#呼叫主程式並代入股票代號與起訖日期
if __name__ == "__main__":   
    starttime = time.clock()
    stkno='2421'
    start = datetime.datetime(2007,10,1)
    end = datetime.datetime(2017,9,30)
    df = main(stkno,start,end) #呼叫主程式
    endtime = time.clock()
    print('程式執行時間 = %d%s' %(round(endtime-starttime), '秒'))