# Python Pandas DataFrame 的 iloc[0] 與 iloc[[0]]


Pandas 是 Python 的一個資料操作庫。

在寫分類器的時，看到別人的範例裡寫了這樣一段：DataFrame.iloc[[0]]，他在[]裡面放入[0]而不是單純的0。

覺得這個取出方法有點神秘，究竟跟DataFrame.iloc[0]有什麼區別呢？

在這邊做個紀錄，順便給pandas.DataFrame的操作方法做個筆記。

參考資料和部分程式碼來源：10 Minutes to pandas - pandas 0.18.0 documentation。

運行環境為 Python 3.5 + Pandas 0.18

```py
import pandas as pd
import numpy as np

# 先建立一個 DataFrame

dates = pd.date_range('20150328', periods=10)   # 日期序列

df = pd.DataFrame(np.random.randn(10, 5), index=dates, columns=list('ABCDE'))

# 各種觀察角度

print(df.index)         # 列出每一個 row 的 index 標籤

print(df.columns)       # 列出每一個 column 的標頭

print(df.values)        # 除去 index 與 column headers 後的數值陣列（np.array）

print(df.describe())    # 根據每一個 column 描述其資料性質（count, mean, std, ...)


# Selection: 直接取出

print(df['A'])          # 這樣會列出整個 DF 的 column 'A'

print(df[0:5])          # 列出 df 的 0, 1, 2, 3, 4 rows

print(df[[0, 1, 2]])    # 列出 df 的 0, 1, 2 三個 columns


# Selection: 使用 .loc 可以做更精細的選出資料

print(df.loc[dates[0]])                           # 根據 row 的 label 選擇

print(df.loc['20140103':'20140105', ['B', 'C']])  # 根據 row labels 和 column names 選擇


# Selection: 使用 .iloc 就可以用 numpy.array/python array 的方式來操作 pandas.DataFrame 

print(df.iloc[3])        # 取出第四個 row

print(df.iloc[0:2, 2:4]) # 取出一個區塊 (row_idx=0, 1 and col_idx=2, 3) 

print(df.iloc[1:4, :])   # 根據 row index 選擇

print(df.iloc[:, 1:3])   # 根據 col index 選擇


# Selection: .iloc[0] 和 .iloc[[0]] 的差別

print(type(df.iloc[0]))   # pandas.core.series.Series

print(type(df.iloc[[0]])) # pandas.core.frame.DataFrame
```
其實 .iloc[0] 和 .iloc[[0]] 的差別就是一個取出的型態是 Series ，另一個是 DataFrame。
