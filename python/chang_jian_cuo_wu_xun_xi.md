# 常見錯誤訊息


## UnicodeEncodeError: 'ascii' codec can 't encode characters in position 0-6: ordinal not in range(128)

通常默認編碼為 ASCII，此編碼接受 0 到 127 這個範圍的編碼，否則報錯

```py
# -*- coding: utf-8 -*-
import sys
reload(sys) 
sys.setdefaultencoding("utf-8")
```



```py
In [1]: u = u'元'

In [2]: u
Out[2]: u'\u5143'

In [3]: print u
元

In [4]: str(u)
---------------------------------------------------------------------------
UnicodeEncodeError                        Traceback (most recent call last)
<ipython-input-4-c27ea7226ccb> in <module>()
----> 1 str(u)

UnicodeEncodeError: 'ascii' codec can't encode character u'\u5143' in position 0: ordinal not in range(128)

In [5]: import sys

In [6]: reload(sys) 
<module 'sys' (built-in)>

In [7]: sys.setdefaultencoding("utf-8")

In [8]: s = str(u)

In [9]: s
'\xe5\x85\x83'

In [10]: print s
元
```