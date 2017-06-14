# 標準差計算

```py
# -*- coding: utf-8 -*-
import numpy as np

def main():
    # データ(気温)
    data = np.array([31,30,27,25,29,34,32,31,30,29])
    std = np.std(data)              # 標準偏差を計算
    print(u"標準偏差："+str(std))   # 結果を表示

if __name__ == '__main__':
    main()
```