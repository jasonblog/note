#RC_7_3: 上網抓資料並畫K線圖
import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter
from matplotlib.finance import quotes_historical_yahoo_ohlc, candlestick_ohlc
#下載資料起迄日, 日期格式與股票代號
start = (2016, 4, 1)
end = (2016, 4, 25)
weekFormatter = DateFormatter('%b %d')  # 例如, Jan 03 2016
quotes = quotes_historical_yahoo_ohlc('AAPL', start, end)
#若抓取的資料是空字串則離開系統
if len(quotes) == 0:
    raise SystemExit
#設定繪圖區域的格式化
fig, ax = plt.subplots()
ax.xaxis_date()
plt.setp(plt.gca().get_xticklabels(), rotation=45, horizontalalignment='right')
#畫K線圖並顯示
candlestick_ohlc(ax, quotes, width=0.6)
plt.show()