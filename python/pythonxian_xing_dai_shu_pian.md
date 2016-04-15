# Python--线性代数篇


讲解Python在线性代数中的应用，包括：

##一、矩阵创建
先导入Numpy模块，在下文中均采用np代替numpy
```py
import numpy as np
```
矩阵创建有两种方法，一是使用np.mat函数或者np.matrix函数，二是使用数组代替矩阵，实际上官方文档建议我们使用二维数组代替矩阵来进行矩阵运算；因为二维数组用得较多，而且基本可取代矩阵。
```py
 >>> a = np.mat([[1, 2, 3], [4, 5, 6]])   #使用mat函数创建一个2X3矩阵
 >>> a
 matrix([[1, 2, 3],
 [4, 5, 6]])
 >>> b = np.matrix([[1, 2, 3], [4, 5, 6]])#np.mat和np.matrix等价
 >>> b
 matrix([[1, 2, 3],
 [4, 5, 6]])
 >>> a.shape     #使用shape属性可以获取矩阵的大小
(2, 3)
```

```py
>>> c = np.array([[1, 2, 3], [4, 5, 6]]) #使用二维数组代替矩阵，常见的操作通用
>>> c#注意c是array类型，而a是matrix类型
array([[1, 2, 3],
[4, 5, 6]])
```
###单位阵的创建

```py
>>> I = np.eye(3)
>>> I
array([[ 1.,  0.,  0.],
       [ 0.,  1.,  0.],
       [ 0.,  0.,  1.]])
```

###矩阵元素的存取操作：

```py
>>> a[0]#获取矩阵的某一行
matrix([[1, 2, 3]])
>>> a[:, 0].reshape(-1, 1)#获取矩阵的某一列
matrix([[1],
        [4]])
>>> a[0, 1]#获取矩阵某个元素
2
```

##二、矩阵乘法和加法
矩阵类型，在满足乘法规则的条件下可以直接相乘

```py
>>> A = np.mat([[1, 2, 3], [3, 4, 5], [6, 7, 8]])#使用mat函数
>>> B = np.mat([[5, 4, 2], [1, 7, 9], [0, 4, 5]])
>>> A   #注意A, B都是matrix类型，可以使用乘号，如果是array则不可以直接使用乘号
matrix([[1, 2, 3],
        [3, 4, 5],
        [6, 7, 8]])
>>> B
matrix([[5, 4, 2],
        [1, 7, 9],
        [0, 4, 5]])
>>> A * B#学过线性代数的都知道：A * B != B * A
matrix([[  7,  30,  35],
        [ 19,  60,  67],
        [ 37, 105, 115]])
>>> B * A
matrix([[ 29,  40,  51],
        [ 76,  93, 110],
        [ 42,  51,  60]])
```

如果是使用数组代替矩阵进行运算则不可以直接使用乘号，应使用dot()函数。dot函数用于矩阵乘法，对于二维数组，它计算的是矩阵乘积，对于一维数组，它计算的是内积。

```py
>>> C = np.array([[1, 2, 3], [3, 4, 5], [6, 7, 8]])
>>> D = np.array([[5, 4, 2], [1, 7, 9], [0, 4, 5]])
>>> C          #C, D都是array类型，不能直接使用乘号，应该使用dot()函数
array([[1, 2, 3],
       [3, 4, 5],
       [6, 7, 8]])
>>> D
array([[5, 4, 2],
       [1, 7, 9],
       [0, 4, 5]])
#>>> C * D, Error, 注意这不是矩阵乘法！！！
>>> np.dot(C, D)#正确的写法，得到的结果和上一段代码的第11行的结果的一样的。
array([[  7,  30,  35],
       [ 19,  60,  67],
       [ 37, 105, 115]])
```

如何理解对于一维数组，它计算的是内积？？？

注意：在线性代数里面讲的维数和数组的维数不同，如线代中提到的n维行向量在Python中是一维数组，而线代中的n维列向量在Python中是一个shape为(n, 1)的二维数组！

第16行，第18行：F是一维数组，G是二维数组，维数不同，个人认为相乘没有意义，但是16行没有错误，18行报错。关于dot()的乘法规则见：NumPy-快速处理数据--矩阵运算

```py
>>> E = np.array([1, 2, 3])
>>> F = np.array([4, 3, 9])
>>> E.shape#E,F都是一维数组
(3,)
>>> np.dot(E, F)
37
>>> np.dot(F, E)
37
>>> G = np.array([4, 3, 9]).reshape(-1, 1)
>>> G
array([[4],
       [3],
       [9]])
>>> G.shape
(3, 1)
>>> np.dot(F, G)#因此dot(F, G)不再是内积，而是一个只有一个元素的数组
array([106])
>>> np.dot(G, F)#ValueError: shapes (3,1) and (3,) not aligned: 1 (dim 1) != 3 (dim 0)
>>> E.shape = (1, -1)#把E改为二维数组
>>> E
array([[1, 2, 3]])
>>> E.shape
(1, 3)
>>> np.dot(G, E)#3×1的G向量乘以1×3的E向量会得到3×3的矩阵
array([[ 4,  8, 12],
       [ 3,  6,  9],
       [ 9, 18, 27]])
```

矩阵的加法运算

```py
>>> A + B#矩阵的加法对matrix类型和array类型是通用的
matrix([[ 6,  6,  5],
        [ 4, 11, 14],
        [ 6, 11, 13]])
>>> C + D
array([[ 6,  6,  5],
       [ 4, 11, 14],
       [ 6, 11, 13]])
```

矩阵的数乘运算
```py
>>> 2 * A#矩阵的数乘对matrix类型和array类型是通用的
matrix([[ 2,  4,  6],
        [ 6,  8, 10],
        [12, 14, 16]])
>>> 2 * C
array([[ 2,  4,  6],
       [ 6,  8, 10],
       [12, 14, 16]])
```

##三、矩阵的转置

```py
>>> A = np.array([[1, 2, 3], [3, 4, 5], [6, 7, 8]])
>>> B = np.array([[5, 4, 2], [1, 7, 9], [0, 4, 5]])
>>> A
array([[1, 2, 3],
       [3, 4, 5],
       [6, 7, 8]])
>>> A.T  #A的转置
array([[1, 3, 6],
       [2, 4, 7],
       [3, 5, 8]])
>>> A.T.T#A的转置的转置还是A本身
array([[1, 2, 3],
       [3, 4, 5],
       [6, 7, 8]])
```

验证矩阵转置的性质：(A±B)'=A'±B'

```py
>>> (A + B).T
array([[ 6,  4,  6],
       [ 6, 11, 11],
       [ 5, 14, 13]])
>>> A.T + B.T
array([[ 6,  4,  6],
       [ 6, 11, 11],
       [ 5, 14, 13]])
```

验证矩阵转置的性质：(KA)'=KA'

```py
>>> 10 * (A.T)
array([[10, 30, 60],
       [20, 40, 70],
       [30, 50, 80]])
>>> (10 * A).T
array([[10, 30, 60],
       [20, 40, 70],
       [30, 50, 80]])
```

验证矩阵转置的性质：(A×B)'= B'×A'

```py
>>> np.dot(A, B).T
array([[  7,  19,  37],
       [ 30,  60, 105],
       [ 35,  67, 115]])
>>> np.dot(B.T, A.T)
array([[  7,  19,  37],
       [ 30,  60, 105],
       [ 35,  67, 115]])
```

##四、方阵的迹
方阵的迹就是主对角元素之和，使用trace()函数获得方阵的迹：
```py
>>> A
array([[1, 2, 3],
       [3, 4, 5],
       [6, 7, 8]])
>>> B
array([[5, 4, 2],
       [1, 7, 9],
       [0, 4, 5]])
>>> np.trace(A)  # A的迹等于A.T的迹
13
>>> np.trace(A.T)
13
>>> np.trace(A+B)# 和的迹 等于 迹的和
30
>>> np.trace(A) + np.trace(B)
30
```

##五、计算行列式
```py
>>> A
array([[1, 2],
       [1, 3]])
>>> np.linalg.det(A)
1.0
```

##六、逆矩阵/伴随矩阵
若A存在逆矩阵(满足det(A) != 0，或者A满秩)，使用linalg.inv求得方阵A的逆矩阵

```py
import numpy as np
>>> A = np.array([[1, -2, 1], [0, 2, -1], [1, 1, -2]])
>>> A
array([[ 1, -2,  1],
       [ 0,  2, -1],
       [ 1,  1, -2]])
>>> A_det = np.linalg.det(A)      #求A的行列式，不为零则存在逆矩阵
>>> A_det
-3.0000000000000004
>>> A_inverse = np.linalg.inv(A)  #求A的逆矩阵
>>> A_inverse
array([[ 1.        ,  1.        ,  0.        ],
       [ 0.33333333,  1.        , -0.33333333],
       [ 0.66666667,  1.        , -0.66666667]])
>>> np.dot(A, A_inverse)          #A与其逆矩阵的乘积为单位阵
array([[ 1.,  0.,  0.],
       [ 0.,  1.,  0.],
       [ 0.,  0.,  1.]])
>>> A_companion = A_inverse * A_det  #求A的伴随矩阵
>>> A_companion
array([[-3., -3., -0.],
       [-1., -3.,  1.],
       [-2., -3.,  2.]])
```

##七、解一元线性方程
使用np.linalg.solve()解一元线性方程组，待解方程为：

```py
 x + 2y +  z = 7
2x -  y + 3z = 7
3x +  y + 2z =18
```

```py
>>> import numpy as np
>>> A = np.array([[1, 2, 1], [2, -1, 3], [3, 1, 2]])
>>> A    #系数矩阵
array([[ 1,  2,  1],
       [ 2, -1,  3],
       [ 3,  1,  2]])
>>> B = np.array([7, 7, 18])
>>> B
array([ 7,  7, 18])
>>> x = np.linalg.solve(A, B)
>>> x
array([ 7.,  1., -2.])
>>> np.dot(A, x)#检验正确性，结果为B
array([  7.,   7.,  18.])
```

使用np.allclose()检测两个矩阵是否相同：

```py
1 >>> np.allclose(np.dot(A, x), B)#检验正确性
2 True
```

使用 help(np.allclose) 查看 allclose() 的用法：

```py
allclose(a, b, rtol=1e-05, atol=1e-08)
    Parameters
    ----------
    a, b : array_like
        Input arrays to compare.
    rtol : float
        The relative tolerance parameter (see Notes).
    atol : float
        The absolute tolerance parameter (see Notes).
    
    Returns
    -------
    allclose : bool
        Returns True if the two arrays are equal within the given
        tolerance; False otherwise.
```

##八、计算矩阵距离
矩阵的距离，这里是的是欧几里得距离，其他距离表示方法我们以后再谈，这里说一下如何计算两个形状相同矩阵之间的距离。

```py
>>> A = np.array([[0, 1], [1, 0]])#先创建两个矩阵
>>> B = np.array([[1, 1], [1, 1]])
>>> C = A - B       #计算距离矩阵C
>>> C
array([[-1,  0],
       [ 0, -1]])
>>> D = np.dot(C, C)#距离矩阵的平方
>>> E = np.trace(D) #计算矩阵D的迹
>>> E
2
>>> E ** 0.5        #将E开平方得到距离
1.4142135623730951
```

关于计算矩阵距离我也不理解。网上看的帖子，先记下来

##九、矩阵的秩
numpy包中的linalg.matrix_rank方法计算矩阵的秩：

```py
>>> import numpy as np
>>> I = np.eye(3)#先创建一个单位阵
>>> I
array([[ 1.,  0.,  0.],
       [ 0.,  1.,  0.],
       [ 0.,  0.,  1.]])
>>> np.linalg.matrix_rank(I)#秩
3
>>> I[1, 1] = 0#将该元素置为0
>>> I
array([[ 1.,  0.,  0.],
       [ 0.,  0.,  0.],
       [ 0.,  0.,  1.]])
>>> np.linalg.matrix_rank(I)#此时秩变成2
2
```

##十、求方阵的特征值特征向量
```py 
>>> import numpy as np
>>> x = np.diag((1, 2, 3))#创建一个对角矩阵！
>>> x
array([[1, 0, 0],
       [0, 2, 0],
       [0, 0, 3]])
>>> a,b = np.linalg.eig(x)#特征值保存在a中，特征向量保存在b中
>>> a
array([ 1.,  2.,  3.])
>>> b
array([[ 1.,  0.,  0.],
       [ 0.,  1.,  0.],
       [ 0.,  0.,  1.]])
```

根据公式 Ax = λx 检验特征值与特征向量是否正确：

```py
for i in range(3):#方法一
    if np.allclose(np.dot(a[i], b[:, i]), x[:, i]):#np.allclose()方法在第七节提到过
        print 'Right'
    else:
        print 'Error'

for i in range(3):#方法二
    if (np.dot(a[i], b[:, i]) == x[:, i]).all():
        print 'Right'
    else:
        print 'Error'
```

注意，如果写成 if np.dot(a[i], b[:, i]) == x[:, i]: 是错误的：(矩阵包含有多个值，应该使用a.any()或者a.all()判断)

 ValueError: The truth value of an array with more than one element is ambiguous. Use a.any() or a.all() 

##十一、判断正定矩阵
- 设M是n阶方阵，如果对任何非零向量z，都有z'Mz> 0，其中z' 表示z的转置，就称M正定矩阵。

- 判定定理1：对称阵A为正定的充分必要条件是：A的特征值全为正。
- 判定定理2：对称阵A为正定的充分必要条件是：A的各阶顺序主子式都为正。
- 判定定理3：任意阵A为正定的充分必要条件是：A合同于单位阵。

下面用定理1判断对称阵是否为正定阵

```py
>>> import numpy as np
>>> A = np.arange(16).reshape(4, 4)
>>> A
array([[ 0,  1,  2,  3],
       [ 4,  5,  6,  7],
       [ 8,  9, 10, 11],
       [12, 13, 14, 15]])
>>> A = A + A.T             #将方阵转换成对称阵
>>> A
array([[ 0,  5, 10, 15],
       [ 5, 10, 15, 20],
       [10, 15, 20, 25],
       [15, 20, 25, 30]])
>>> B = np.linalg.eigvals(A)#求B的特征值，注意：eig()是求特征值特征向量
>>> B
array([  6.74165739e+01 +0.00000000e+00j,
        -7.41657387e+00 +0.00000000e+00j,
         2.04219701e-15 +3.94306094e-15j,
         2.04219701e-15 -3.94306094e-15j])

if np.all(B>0):             #判断是不是所有的特征值都大于0，用到了all函数，显然对称阵A不是正定的
    print 'Yes'
```

创建一个对角元素都为正的对角阵，它一定是正定的：

```py
>>> A = np.diag((1, 2, 3))#创建对角阵，其特征值都为正
>>> B = np.linalg.eigvals(A)#求特征值
>>> B
array([ 1.,  2.,  3.])
>>> if np.all(B>0):#判断特征值是否都大于0
    print 'Yes'
```

网上查到更简便的方法是对对称阵进行cholesky分解，如果像这样没有提示出错，就说明它是正定的。如果提示出错，就说明它不是正定矩阵，你可以使用try函数捕获错误值：

```py
# -*- coding: utf-8 -*-
import numpy as np

A = np.arange(16).reshape(4, 4)
A = A + A.T
print A
try:
    B = np.linalg.cholesky(A)
except :
    print ('不是正定矩阵，不能进行cholesky分解。')
```

当不能进行cholesky分解时，出现的异常是： LinAlgError: Matrix is not positive definite ，但是但是LinAlgError不是Python标准异常，因此不能使用这条语句。

```py
1 except LinAlgError as reason:
2     print ('不是正定矩阵，不能进行cholesky分解。\n出错原因是：' + str(reason))
```