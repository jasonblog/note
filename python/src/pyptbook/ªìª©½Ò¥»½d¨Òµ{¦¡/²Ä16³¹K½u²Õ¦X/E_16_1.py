import pymysql.cursors
import pandas as pd
import numpy as np
import datetime
import time
#引用talib相關套件
import talib
from talib import MA_Type
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


def trend_judgement_2(df, stkno, count, acmroi, winnum, winfact, trend_type):
    #MA_Type: 0=SMA, 1=EMA, 2=WMA, 3=DEMA, 4=TEMA, 5=TRIMA, 6=KAMA, 7=MAMA, 8=T3 (Default=SMA)
    close = np.array(df['close'], dtype=float)
    SMA = talib.SMA(close,3) #close 代進WMA方法做計算
    WMA = talib.WMA(close,3) #close 代進WMA方法做計算
    EMA = talib.EMA(close,10)
    df['SMA'] = SMA
    df['WMA'] = WMA
    df['EMA'] = EMA
    row = len(df)
    for i in range(row):
        #趨勢1 SMA3
        if(trend_type == 'SMA3'):
            trend_status = SMA3(df, i)
        #趨勢2 EMA10
        elif(trend_type == 'EMA10'):
            trend_status = EMA10(df, i)
        #趨勢3 Levy趨勢
        elif(trend_type == 'Levy'):
            trend_status = Levy(df, i)

    return df
#趨勢1 SMA3
def SMA3(df, i):
    trend_status = None    
    if(i>=6):
        if(df['SMA'].iloc[i]>df['SMA'].iloc[i-1]>df['SMA'].iloc[i-2]>df['SMA'].iloc[i-3]>df['SMA'].iloc[i-4]>df['SMA'].iloc[i-5]>df['SMA'].iloc[i-6]):
            trend_status = True
            print('第 {0} 筆 上漲趨勢SMA: {1}'.format(i, df['SMA'].iloc[i]))          
        elif(df['SMA'].iloc[i]<df['SMA'].iloc[i-1]<df['SMA'].iloc[i-2]<df['SMA'].iloc[i-3]<df['SMA'].iloc[i-4]<df['SMA'].iloc[i-5]<df['SMA'].iloc[i-6]):
            trend_status = False
            print('第 {0} 筆 下降趨勢SMA: {1}'.format(i, df['SMA'].iloc[i]))
    return trend_status
#趨勢2 EMA10
def EMA10(df, i):
    trend_status = None
    if(i>=10):
        if(df['close'].iloc[i]>df['EMA'].iloc[i]):
            trend_status = True
            print('第 {0} 筆 上漲趨勢EMA: {1}'.format(i, df['EMA'].iloc[i]))
        elif(df['close'].iloc[i]<df['EMA'].iloc[i]):
            trend_status = False
            print('第 {0} 筆 下降趨勢EMA: {1}'.format(i, df['EMA'].iloc[i])) 
    return trend_status
#趨勢3 Levy趨勢
def Levy(df, i):
    trend_status = None
    if(i>=131):
        total_change = 0            
        today_change = df['close'].iloc[i]/df['open'].iloc[i]
        before_change = df['close'].iloc[i-6]/df['open'].iloc[i-6]            
        for j in range(i-131, i):
            total_change +=  df['close'].iloc[j]/df['open'].iloc[j]
        average_change = total_change/131.0

        if((today_change-before_change)/before_change > 6*average_change):
            trend_status = True
            print('第 {0} 筆 上漲趨勢Levy: {1}'.format(i, today_change))
        elif((today_change-before_change)/before_change < 6*average_change):
            trend_status = False
            print('第 {0} 筆 下降趨勢Levy: {1}'.format(i, today_change))
    return trend_status
#趨勢判斷
def trend_judgement_1(df, stkno, count, acmroi, winnum, winfact, trend_type):
    #MA_Type: 0=SMA, 1=EMA, 2=WMA, 3=DEMA, 4=TEMA, 5=TRIMA, 6=KAMA, 7=MAMA, 8=T3 (Default=SMA)
    close = np.array(df['close'], dtype=float)
    SMA = talib.SMA(close,3) #close 代進WMA方法做計算
    df['SMA'] = SMA
    row = len(df)
    for i in range(row):
        #趨勢1 SMA3
        if(trend_type == 'SMA3'):
            trend_status = SMA3(df, i)

    return df

def trend_judgement(df, stkno, count, acmroi, winnum, winfact, trend_type):
    #MA_Type: 0=SMA, 1=EMA, 2=WMA, 3=DEMA, 4=TEMA, 5=TRIMA, 6=KAMA, 7=MAMA, 8=T3 (Default=SMA)
    close = np.array(df['close'], dtype=float)
    SMA = talib.SMA(close,3) #close 代進WMA方法做計算
    WMA = talib.WMA(close,3) #close 代進WMA方法做計算
    EMA = talib.EMA(close,10)
    df['SMA'] = SMA
    df['WMA'] = WMA
    df['EMA'] = EMA
    #設定初始值
    df['XBuy'] = np.nan
    df['YBuy'] = np.nan
    df['XSell'] = np.nan    
    df['YSell'] = np.nan
    row = len(df)
    flag = False    
    buyprice=[]
    sellprice=[] 
    win = 0
    loss = 0
    for i in range(row):
        #趨勢1 SMA3
        if(trend_type == 'SMA3'):
            trend_status = SMA3(df, i)
        #趨勢2 EMA10
        elif(trend_type == 'EMA10'):
            trend_status = EMA10(df, i)
        #趨勢3 Levy趨勢
        elif(trend_type == 'Levy'):
            trend_status = Levy(df, i)    
        else:
            trend_status = None

        #判斷有趨勢變化
        if(trend_status!=None):
            #進行八種交易策略判斷
            trade_result = trade_judgement(df, i, trend_status)
            if(trade_result == True and flag == False):        
                df['XBuy'].iloc[i] =  df['dates'].iloc[i]
                buyprice = df['close'].iloc[i]
                df['YBuy'].iloc[i] = buyprice
                flag = True              
            elif(trade_result == False and flag == True):                   
                df['XSell'].iloc[i] = df['dates'].iloc[i]
                sellprice = df['close'].iloc[i]                
                df['YSell'].iloc[i] = sellprice
                flag = False
                count += 1
                [roi, winnum] = roical(buyprice, sellprice, winnum)
                acmroi += roi
                [loss, win] = winfactor(buyprice, sellprice, loss, win)
            elif(i==(len(df)-1) and flag == True):
                df['XSell'].iloc[i] = df['dates'].iloc[i]
                sellprice = df['close'].iloc[i]
                df['YSell'].iloc[i] = sellprice
                count += 1
                [roi, winnum] = roical(buyprice, sellprice, winnum)
                acmroi += roi                
    if (loss == 0):
        loss = 1
    winvar = win / loss
    if (count == 0):
        count = 0.01

    df_result = pd.DataFrame([[stkno, str(count),str(round(acmroi*100, 2))+'%',str(round((winnum/count)*100,2))+'%', str(round(winvar, 2))]],columns=['股票代號','交易次數','累計報酬率','勝率','獲利因子'])
    return df, df_result

#交易策略
def trade_judgement(df, i, trend_status):

    #是否達成任一交易策略
    trade = False
    ##上升趨勢
    if(trend_status == True):
        #TWS
        if(trade == False):
            ans = 0
            if((df['close'].iloc[i]>df['open'].iloc[i]) and (df['close'].iloc[i-1]>df['open'].iloc[i-1]) and (df['close'].iloc[i-2]>df['open'].iloc[i-2])):
                ans +=1
            if((df['close'].iloc[i]>df['close'].iloc[i-1]) and (df['close'].iloc[i-1]>df['close'].iloc[i-2])):
                ans +=1
            if((df['close'].iloc[i-2]>df['open'].iloc[i-1]) and (df['open'].iloc[i-1]>df['open'].iloc[i-2])):
                ans +=1
            if((df['close'].iloc[i-1]>df['open'].iloc[i]) and (df['open'].iloc[i]>df['open'].iloc[i-1])):
                ans +=1
            #符合以上四項TWS標準
            if(ans == 4):
                trade = True
        #TIU
        if(trade == False):
            ans = 0
            if((df['open'].iloc[i-2]>df['close'].iloc[i-2]) and (df['open'].iloc[i-2]>=df['open'].iloc[i-1]) and (df['open'].iloc[i-1]>df['close'].iloc[i-2])):
                ans +=1
            if((df['open'].iloc[i-2]>df['close'].iloc[i-1]) and (df['close'].iloc[i-1]>=df['close'].iloc[i-2])):
                ans +=1
            if(df['close'].iloc[i]>df['open'].iloc[i]):
                ans +=1
            if(df['close'].iloc[i]>df['open'].iloc[i-2]):
                ans +=1
            if(ans == 4):
                trade = True
        #TOU
        if(trade == False):
            ans = 0
            if((df['open'].iloc[i-2]>df['close'].iloc[i-2]) and (df['close'].iloc[i-1]>df['open'].iloc[i-2])):
                ans +=1
            if((df['open'].iloc[i-2]>df['close'].iloc[i-2]) and (df['close'].iloc[i-2]>df['open'].iloc[i-1])):
                ans +=1
            if(df['close'].iloc[i]>df['open'].iloc[i]):
                ans +=1
            if(df['close'].iloc[i]>df['close'].iloc[i-1]):
                ans +=1
            if(ans == 4):
                trade = True
        #MS
        if(trade == False):
            ans = 0
            if((df['open'].iloc[i-2]>df['close'].iloc[i-2]) and (abs(df['open'].iloc[i-1]-df['close'].iloc[i-1])>0)):
                ans +=1
            if((df['close'].iloc[i-2]>df['close'].iloc[i-1]) and (df['close'].iloc[i-2]>df['open'].iloc[i-1])):
                ans +=1
            if(df['close'].iloc[i]>df['open'].iloc[i]):
                ans +=1
            if(df['close'].iloc[i]>(df['close'].iloc[i-2]+(df['open'].iloc[i-2]-df['close'].iloc[i-2])/2)):
                ans +=1
            if(ans == 4):
                trade = True
        if(trade == True):
            return True
        elif(trade == False):
            return None

    ##下降趨勢
    elif(trend_status == False):
        #TBC
        if(trade == False):
            ans = 0
            if((df['close'].iloc[i]<df['open'].iloc[i]) and (df['close'].iloc[i-1]<df['open'].iloc[i-1]) and (df['close'].iloc[i-2]<df['open'].iloc[i-2])):
                ans +=1
            if((df['close'].iloc[i]<df['close'].iloc[i-1]) and (df['close'].iloc[i-1]<df['close'].iloc[i-2])):
                ans +=1
            if((df['close'].iloc[i-2]<df['open'].iloc[i-1]) and (df['open'].iloc[i-1]<df['open'].iloc[i-2])):
                ans +=1
            if((df['close'].iloc[i-1]<df['open'].iloc[i]) and (df['open'].iloc[i]<df['open'].iloc[i-1])):
                ans +=1
            if(ans == 4):
                trade = True
        #TID
        if(trade == False):
            ans = 0
            if((df['open'].iloc[i-2]<df['close'].iloc[i-2]) and (df['open'].iloc[i-2]<=df['open'].iloc[i-1]) and (df['open'].iloc[i-1]<df['close'].iloc[i-2])):
                ans +=1
            if((df['open'].iloc[i-2]<df['close'].iloc[i-1]) and (df['close'].iloc[i-1]<=df['close'].iloc[i-2])):
                ans +=1
            if(df['close'].iloc[i]<df['open'].iloc[i]):
                ans +=1
            if(df['close'].iloc[i]<df['open'].iloc[i-2]):
                ans +=1
            if(ans == 4):
                trade = True
        #TOD
        if(trade == False):
            ans = 0
            if((df['open'].iloc[i-2]<df['close'].iloc[i-2]) and (df['close'].iloc[i-2]<df['open'].iloc[i-1])):
                ans +=1
            if((df['open'].iloc[i-2]<df['close'].iloc[i-2]) and (df['close'].iloc[i-1]<df['open'].iloc[i-2])):
                ans +=1
            if(df['close'].iloc[i]<df['open'].iloc[i]):
                ans +=1
            if(df['close'].iloc[i]<df['close'].iloc[i-1]):
                ans +=1
            if(ans == 4):
                trade = True
        #ES
        if(trade == False):
            ans = 0
            if(df['open'].iloc[i-2]<df['close'].iloc[i-2] and (abs(df['open'].iloc[i-1]-df['close'].iloc[i-1])>0)):
                ans +=1
            if((df['close'].iloc[i-2]<df['close'].iloc[i-1]) and (df['close'].iloc[i-2]<df['open'].iloc[i-1])):
                ans +=1
            if(df['close'].iloc[i]<df['open'].iloc[i]):
                ans +=1
            if(df['close'].iloc[i]<(df['close'].iloc[i-2]-(df['close'].iloc[i-2]-df['open'].iloc[i-2])/2)):
                ans +=1
            if(ans == 4):
                trade = True

        if(trade == True):
            return False
        elif(trade == False):
            return None

#計算報酬率與勝率
def roical(buyprice, sellprice, winnum):
    roi = ((sellprice - buyprice) / buyprice)
    if roi > 0:
        winnum+=1
    return (roi, winnum)
#計算獲利因子要用的累計獲利金額與損失金額
def winfactor(buyprice, sellprice, loss, win):
    payoff = buyprice - sellprice
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
            #轉換資料庫抓取的資料型態
            df['open'] = df['open'].astype('float64')
            df['high'] = df['high'].astype('float64')
            df['close'] = df['close'].astype('float64')
            df['low'] = df['low'].astype('float64')
            return df

    except Exception as e:
        print("Exeception occured:{}".format(e))

    finally:
        connection.close()

def main(stkno, get_type, trend_type): 
    df = select_db(stkno)    
    #進行趨勢判斷
    df, df_result = trend_judgement(df, stkno, count = 0, acmroi = 0, winnum = 0, winfact = 0, trend_type = trend_type)
    if(get_type == 'one'):
        showMAplot(df)

    return (df_result)