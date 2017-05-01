# 数据操作（Pandas） 完整


##1.1. pandas 对象

引入 pandas 等包，DataFrame、Series 属于常用的，所以直接引入

```py
%matplotlib inline
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from pandas import Series, DataFrame
```

DataFrame 对象：Pandas DataFrame 是一个表格型的数据结构，有行索引也有列索引


```py
from IPython.display import Image
Image(filename='../../image/DataFrame.png', width=400)
```
![](./images/base_01_pandas_5_0.png)


Series 对象：类似于一维数组的对象，由一组同样 type 的数组和索引组成


```py
s1 = Series(range(0,4)) # -> 0, 1, 2, 3
s2 = Series(range(1,5)) # -> 1, 2, 3, 4
s3 = s1 + s2 # -> 1, 3, 5, 7
s4 = Series(['a','b'])*3 # -> 'aaa','bbb'
```

index 对象：即 Series 和 DataFrame 的索引


```py
# 获取索引
df = DataFrame(s1)
idx = s1.index
idx = df.columns # the column index
idx = df.index # the row index
```

```py

# 索引的一些特性
b = idx.is_monotonic_decreasing
b = idx.is_monotonic_increasing
b = idx.has_duplicates
i = idx.nlevels # multi-level indexe
```

```py
# 索引的一些方法
a = idx.values # get as numpy array
l = idx.tolist() # get as a python list
# idx = idx.astype(dtype) # change data type
# b = idx.equals(other) # check for equality 看看是否是相同的索引
```

```py
# union of two indexes 合并两个索引
# idx = idx.union(other)

idx1 = pd.Index([1, 2, 3, 4])
idx2 = pd.Index([3, 4, 5, 6])
idx1.union(idx2)
```

```py
Int64Index([1, 2, 3, 4, 5, 6], dtype='int64')
```

```py
i = idx.nunique() # number unique labels
label = idx.min() # minimum label
label = idx.max() # maximum label
```

创建 Series 和 DataFrame

http://pandas.pydata.org/pandas-docs/stable/dsintro.html

##1.2. DataFrame 入门

```py
df = DataFrame(np.random.randn(10, 4), columns=['A', 'B', 'C', 'D'])
```

DataFrame 的一些实用查看方法

```py
df.info()
```


```py

<class 'pandas.core.frame.DataFrame'>
RangeIndex: 10 entries, 0 to 9
Data columns (total 4 columns):
A    10 non-null float64
B    10 non-null float64
C    10 non-null float64
D    10 non-null float64
dtypes: float64(4)
memory usage: 392.0 bytes
```

```py
n=4
dfh = df.head(n) # 看前 n 行
```


```py
dft = df.tail(n) # 看后 n 行
```

```py
dfs = df.describe() # 各类统计信息
```

```py
top_left_corner_df = df.iloc[:5, :5]
```

```py
dfT = df.T # transpose rows and cols
```

## DataFrame index 的一些特性

```py
l = df.axes # list row and col indexes
l
```

```py
[RangeIndex(start=0, stop=10, step=1),
Index([u'A', u'B', u'C', u'D'], dtype='object')]
```

```py
(r, c) = df.axes # from above
```

```py
s = df.dtypes # Series column data types
```

```py
A    float64
B    float64
C    float64
D    float64
dtype: object
```

```py
b = df.empty # True for empty DataFrame
b
```

```py
False
```

```py
i = df.ndim # number of axes (2)
i
```
```py
2
```

```py
t = df.shape # (row-count, column-count)
t
```

```py
(10, 4)
```

```py
(r, c) = df.shape # from above
(r, c)
```

```py
(10, 4)
```

```py
i = df.size # row-count * column-count
i
```

```py
40
```

```py
a = df.values # get a numpy array for df
```