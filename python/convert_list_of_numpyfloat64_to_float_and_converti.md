# Convert list of numpy.float64 to float and Converting strings to floats in a DataFrame


```py
import pandas as pd
h = pd.Series(['15', '21.0', '33.0'])
l = pd.Series(['1', '2.0', '3.0'])

# Converting strings to floats in a DataFrame  using to_numeric
h = pd.to_numeric(h)
l = pd.to_numeric(l)
s = h - l

print type(s)
print s

# Convert list of numpy.float64 to float using tolist
s = s.tolist()
print type(s)
print s

```