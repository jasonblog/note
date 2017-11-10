# NumPy中array和matrix用於矩陣乘法時的區別


在NumPy中，array用於表示通用的N維數組，matrix則特定用於線性代數計算。array和matrix都可以用來表示矩陣，二者在進行乘法操作時，有一些不同之處。

使用array時，運算符 * 用於計算`數量`積（點乘），函數 dot() 用於計算`矢量`積（叉乘），例子如：


```py
import numpy as np
 
a = np.array([[1, 2], [3, 4]])
b = np.array([[5, 6], [7, 8]])
 
print 'a * b = \n', a * b
print 'np.dot(a, b) = \n', np.dot(a, b)
```

運行結果為：

```sh
a * b = 
[[ 5 12]
 [21 32]]
np.dot(a, b) = 
[[19 22]
 [43 50]]
```

可見，當a和b為array時， ```a * b``` 計算了a和b的數量積（對應Matlab的 ```a .* b``` ）， ```dot(a, b)``` 計算了a和b的矢量積（對應Matlab的 a * b ）。

與array不同的是，使用matrix時，運算符 * 用於計算矢量積，函數 multiply() 用於計算數量積，例子如：


```py
import numpy as np
 
a = np.mat('1 2; 3 4')
b = np.mat('5 6; 7 8');
 
print 'a * b = \n', a * b
print 'np.multiply(a, b) = \n', np.multiply(a, b)
```

運行結果為：


```sh
a * b = 
[[19 22]
 [43 50]]
np.multiply(a, b) = 
[[ 5 12]
 [21 32]]
 ```
 
 可見，當a和b為matrix時， a * b 計算了a和b的矢量積， multiply(a, b) 計算了a和b的數量積。當使用matrix時，無論是生成矩陣還是計算，Numpy的風格和Matlab更加貼近，降低了語言切換時的負擔。