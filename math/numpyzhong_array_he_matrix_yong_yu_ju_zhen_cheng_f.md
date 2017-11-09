# NumPy中array和matrix用于矩阵乘法时的区别


在NumPy中，array用于表示通用的N维数组，matrix则特定用于线性代数计算。array和matrix都可以用来表示矩阵，二者在进行乘法操作时，有一些不同之处。

使用array时，运算符 * 用于计算`数量`积（点乘），函数 dot() 用于计算`矢量`积（叉乘），例子如：


```py
import numpy as np
 
a = np.array([[1, 2], [3, 4]])
b = np.array([[5, 6], [7, 8]])
 
print 'a * b = \n', a * b
print 'np.dot(a, b) = \n', np.dot(a, b)
```

运行结果为：

```sh
a * b = 
[[ 5 12]
 [21 32]]
np.dot(a, b) = 
[[19 22]
 [43 50]]
```

可见，当a和b为array时， ```a * b``` 计算了a和b的数量积（对应Matlab的 ```a .* b``` ）， ```dot(a, b)``` 计算了a和b的矢量积（对应Matlab的 a * b ）。

与array不同的是，使用matrix时，运算符 * 用于计算矢量积，函数 multiply() 用于计算数量积，例子如：


```py
import numpy as np
 
a = np.mat('1 2; 3 4')
b = np.mat('5 6; 7 8');
 
print 'a * b = \n', a * b
print 'np.multiply(a, b) = \n', np.multiply(a, b)
```

运行结果为：


```sh
a * b = 
[[19 22]
 [43 50]]
np.multiply(a, b) = 
[[ 5 12]
 [21 32]]
 ```
 
 可见，当a和b为matrix时， a * b 计算了a和b的矢量积， multiply(a, b) 计算了a和b的数量积。当使用matrix时，无论是生成矩阵还是计算，Numpy的风格和Matlab更加贴近，降低了语言切换时的负担。