# Add a row to dataframe

```py
from pandas import DataFrame as df  
mydf = df([1.1, 1.1, 1.1]).T  
arow2 = [2.2, 2.2, 2.2]  
mydf.loc[len(mydf)] = arow2  
print(mydf)
```