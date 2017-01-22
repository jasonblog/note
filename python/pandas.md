# pandas


```py
import pandas as pd
import numpy as np

data = np.random.rand(10, 5)
df = pd.DataFrame(data, columns=list("ABCDE"))
print df 

df.columns = list("12345")

df.index = list("ABCDEFGHIJ")

print df

print df.index.values
print df['1']

df.index = list("0123456789")
```


