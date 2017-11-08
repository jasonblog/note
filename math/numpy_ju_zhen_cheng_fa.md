# NumPy 矩阵乘法


先上结论：NumPy 中对 array 使用 multiply() 或 * 操作符并不是矩阵乘法，正确的用法是 dot() 或 matmul()，或者对 matrix 使用 *。

根据 numpy.multiply 文档说明，multiply() 是 element-wise 的乘法，换句话说，是把两个入参数组中对应元素进行相乘。

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

在上面的代码中，ma 是 2×3 矩阵，mb 是 3×2 矩阵，预期二者相乘的结果应该是一个 2×2 矩阵，但实际运行会报异常。在 multiply() 看来，ma 和 mb 只是两个形状不同的二维数组，因此无法把对应的元素进行相乘。c * d 符合 element-wise 相乘的结果。

正确的用法是什么呢？

- numpy.dot，对于二维数组，dot() 等价于矩阵乘法

```py
>>> ma.dot(mb)
array([[20, 14],
    [56, 41]])
```

- numpy.matmul，从 NumPy 1.10.0 开始被引入

```py
>>> matmul(ma,mb)
array([[20, 14],
    [56, 41]])
```

- 将 array 类型转换为 matrix 再使用 * 操作符

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

- python3 的 @ 操作符，参考这里，由于不用 python3，没试过