# NumPy 矩陣乘法


先上結論：NumPy 中對 array 使用 multiply() 或 * 操作符並不是矩陣乘法，正確的用法是 dot() 或 matmul()，或者對 matrix 使用 *。

根據 numpy.multiply 文檔說明，multiply() 是 element-wise 的乘法，換句話說，是把兩個入參數組中對應元素進行相乘。

```py
>>> from numpy import *
>>> a = [1, 2, 3, 4, 5, 6]
>>> b = [6, 5, 4, 3, 2, 1]
>>> ma = reshape(a, (2, 3))
>>> mb = reshape(b, (3, 2))
>>> ma
array([[1, 2, 3],
       [4, 5, 6]])
>>> mb
array([[6, 5],
       [4, 3],
       [2, 1]])
>>> multiply(ma, mb)
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
ValueError: operands could not be broadcast together with shapes (2,3) (3,2)
>>> c = arange(6).reshape(2,3)
>>> d = arange(6).reshape(2,3)
>>> c
array([[0, 1, 2],
       [3, 4, 5]])
>>> d
array([[0, 1, 2],
       [3, 4, 5]])
>>> c * d
array([[ 0,  1,  4],
       [ 9, 16, 25]])

```

在上面的代碼中，ma 是 2×3 矩陣，mb 是 3×2 矩陣，預期二者相乘的結果應該是一個 2×2 矩陣，但實際運行會報異常。在 multiply() 看來，ma 和 mb 只是兩個形狀不同的二維數組，因此無法把對應的元素進行相乘。c * d 符合 element-wise 相乘的結果。

正確的用法是什麼呢？

- numpy.dot，對於二維數組，dot() 等價於矩陣乘法

```py
>>> ma.dot(mb)
array([[20, 14],
    [56, 41]])
```

- numpy.matmul，從 NumPy 1.10.0 開始被引入

```py
>>> matmul(ma,mb)
array([[20, 14],
    [56, 41]])
```

- 將 array 類型轉換為 matrix 再使用 * 操作符

```py
>>> np.asmatrix(c) * np.asmatrix(d.reshape(3,2))
matrix([[10, 13],
     [28, 40]])
>>> np.asmatrix(c) * np.asmatrix(d.T)
matrix([[ 5, 14],
     [14, 50]])
>>> np.asmatrix(c) * np.asmatrix(d.T)
matrix([[ 5, 14],
     [14, 50]])
>>> np.mat(c) * np.mat(d).T
matrix([[ 5, 14],
     [14, 50]])
>>> np.mat(c) * np.mat(d.T)
matrix([[ 5, 14],
     [14, 50]])
```

- python3 的 @ 操作符，參考這裡，由於不用 python3，沒試過