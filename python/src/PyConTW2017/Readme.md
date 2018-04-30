# PyCon TW 2017 - 土炮股票分析系統

投影片網址： [https://goo.gl/JVLhRh](https://goo.gl/JVLhRh)

影片網址： [https://bit.ly/2qASbEu](https://bit.ly/2qASbEu)

這個主題主要是想要藉由實際的例子引起財管相關的學生對 Python 的學習興趣。所以，這個分享裡面的例子，被做了很多的簡化，如果真的想要自己做一套分析系統，範例裡面不足的地方，使用者需要自行斟酌如何加以修改。

需要注意的地方：

* Pandas data-reader 裡面關於 Yahoo 的 API 在 2017/05 之後已經無法使用，使用 google 作為 data source 時，無法使用{股票編號}.tw 的方式來讀取台灣股票的歷史股價。替代的方式是使用 quandl 來做存取。這部分請見 quandl 那份 .ipynb 的程式。
* 頻繁的 request 歷史股價可能會被當作是攻擊，而導致資料存取失敗。
* 資料可以先讀到資料庫內，方便以後 off-line 的使用。如果是使用 mongoDB，可以參考 [dbtools](https://github.com/victorgau/dbtools)。不過這個也是簡易版。
* 範例中的策略並沒有考慮股票的 split/merge 或股利的發放等等，對股價造成的改變。實際運用上需要做一些調整，如：
```
df['open'] = df['open'] * df['adj close'] / df['close']
```
* 範例裡面的策略，並沒有考慮實際可能的買價，而是以訊號產生當天的開盤價為買價，或訊號產生前一天的收盤價為賣價。這個實際上是不太合理的，可能會讓實際回測的結果大大的優於實際操作的結果。可以比較下面這兩行程式的回測結果，再思考一下怎麼修改。
 ```
 df['positions'] = df['signals'].cumsum()

 # 產生訊號隔天買進或賣出
 df['positions'] = df['signals'].cumsum().shift()
 ```

還有很多問題，請在讀完 code 之後，自行修改... :p

相關 PyCon TW 2017 講題：

* [[自py系列2] 投資策略驗證系統 - Kilik](https://tw.pycon.org/2017/en-us/events/talk/322153690393739347/)
* [Numpy for dummy - Tim](https://tw.pycon.org/2017/en-us/events/talk/348362827431411846/)
* [Practical Data Transformation and Analysis with Pandas - Zong-han, Xie ](https://tw.pycon.org/2017/en-us/events/talk/347901318163071105/)
* [遺傳演算法最佳化高頻交易策略 - 林萍珍 教授](https://tw.pycon.org/2017/en-us/events/talk/345417142851600500/)
* [OpenCLGA - Run Your GA on Top of PyOpenCL - John](https://tw.pycon.org/2017/en-us/events/talk/322244195077062740/)
* [用Python成為網路投資王 - 潘穎達、陳柏瑋](https://tw.pycon.org/2017/en-us/events/talk/320211463073431632/)

相關資料：

* [Python Data Analysis Library](http://pandas.pydata.org/)
* [Pandas Cookbook](http://pandas.pydata.org/pandas-docs/stable/cookbook.html)
* [Python for Data Analysis - GitHub - Wes McKinney](https://github.com/wesm/pydata-book)
* [Python for Finance - Financial Time Series](https://github.com/yhilpisch/py4fi/blob/master/ipython3/06_Financial_Time_Series.ipynb)
* [Pandas 入門教學講義](https://github.com/Wei1234c/Introduction_to_Pandas)
* [Pandas API Reference](http://pandas.pydata.org/pandas-docs/stable/api.html)