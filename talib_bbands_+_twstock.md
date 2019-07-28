# talib BBands + twstock


```py
import twstock
import numpy
import talib
from talib import MA_Type
import matplotlib.pyplot as plt
from matplotlib import rc

stock = twstock.Stock('4991')

#print(stock.fetch(2019, 7))
#print(stock.fetch_from(2019, 7)[0])
#print(stock.fetch_from(2019, 7)[0].close)
#print(stock.fetch_from(2019, 7)[1])
#print(stock.fetch_from(2019, 7)[1].close)
#print(type(stock.fetch_from(2019, 7)))

#close = numpy.random.random(31)
close = []
data = stock.fetch_from(2019, 5)
print(data)
for item in data:
    close.append(item.close)

print(len(close))
close = numpy.array(close)

print("MA_Type.T3 = " + str(MA_Type.T3))

# 創建布林通道：  週期 20日（＝日K月均線）、2.1 個標準差; 如果只有20天收盤價只有算出一組值
upper, middle, lower = talib.BBANDS(close, 
                                    timeperiod=20, 
                                    nbdevup=2.1, 
                                    nbdevdn=2.1, 
                                    # Moving average type: simple moving average here
                                    matype=0)
print(upper)
print(middle)
print(lower)

plt.figure(figsize=(15,7))
plt.plot(middle, 'r', alpha=0.3)
plt.plot(upper, 'g', alpha=0.3)
plt.plot(lower, 'g', alpha=0.3)
plt.show()

#def BBandMA(df, count, acmroi, winnum, winfact):
    #計算BBand上, 中, 下線
    #close = np.array(df['close'], dtype=float)
    #upper, middle, lower = talib.BBANDS(close, timeperiod=20, nbdevup=0.0001, nbdevdn=0.0001, matype=MA_Type.T3)



```