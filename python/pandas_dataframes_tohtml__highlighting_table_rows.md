# Pandas Dataframes to_html: Highlighting table rows


```py
import numpy as np
import pandas as pd

HEADER = '''
<html>
    <head>
        <style>
            .df tbody tr:last-child { background-color: #FF0000; }
        </style>
    </head>
    <body>
'''
FOOTER = '''
    </body>
</html>
'''

df = pd.DataFrame({'a': np.arange(10), 'b': np.random.randn(10)})
with open('test.html', 'w') as f:
    f.write(HEADER)
    f.write(df.to_html(classes='df'))
    f.write(FOOTER)
```

