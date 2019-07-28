# talib BBands + twstock


```py
import twstock
import numpy
import talib
from talib import MA_Type

stock = twstock.Stock('4991')

#print(stock.fetch(2019, 7))
#print(stock.fetch_from(2019, 7)[0])
#print(stock.fetch_from(2019, 7)[0].close)
#print(stock.fetch_from(2019, 7)[1])
#print(stock.fetch_from(2019, 7)[1].close)
#print(type(stock.fetch_from(2019, 7)))

#close = numpy.random.random(31)
close = []
data = stock.fetch_from(2019, 6)
print(data)
for item in data:
    close.append(item.close)

print(len(close))
close = numpy.array(close)

upper, middle, lower = talib.BBANDS(close, timeperiod=5, nbdevup=2.1, nbdevdn=2.1, matype=0)
print(upper)
print(middle)
print(lower)

#def BBandMA(df, count, acmroi, winnum, winfact):
    #計算BBand上, 中, 下線
    #close = np.array(df['close'], dtype=float)
    #upper, middle, lower = talib.BBANDS(close, timeperiod=20, nbdevup=0.0001, nbdevdn=0.0001, matype=MA_Type.T3)


```