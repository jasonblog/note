# 常見錯誤訊息


## UnicodeEncodeError: 'ascii' codec can 't encode characters in position 0-6: ordinal not in range(128)


```py
# -*- coding: utf-8 -*-
import sys
reload(sys) 
sys.setdefaultencoding("utf-8")
```


