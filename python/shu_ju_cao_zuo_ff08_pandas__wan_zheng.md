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