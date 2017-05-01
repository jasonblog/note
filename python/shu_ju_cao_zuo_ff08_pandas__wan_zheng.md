# 數據操作（Pandas） 完整


##1.1. pandas 對象

引入 pandas 等包，DataFrame、Series 屬於常用的，所以直接引入

```py
%matplotlib inline
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from pandas import Series, DataFrame
```

DataFrame 對象：Pandas DataFrame 是一個表格型的數據結構，有行索引也有列索引


```py
from IPython.display import Image
Image(filename='../../image/DataFrame.png', width=400)
```
![](./images/base_01_pandas_5_0.png)


Series 對象：類似於一維數組的對象，由一組同樣 type 的數組和索引組成


```py
s1 = Series(range(0,4)) # -> 0, 1, 2, 3
s2 = Series(range(1,5)) # -> 1, 2, 3, 4
s3 = s1 + s2 # -> 1, 3, 5, 7
s4 = Series(['a','b'])*3 # -> 'aaa','bbb'
```