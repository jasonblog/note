# Python--線性代數篇


講解Python在線性代數中的應用，包括：

##一、矩陣創建
先導入Numpy模塊，在下文中均採用np代替numpy
```py
import numpy as np
```
矩陣創建有兩種方法，一是使用np.mat函數或者np.matrix函數，二是使用數組代替矩陣，實際上官方文檔建議我們使用二維數組代替矩陣來進行矩陣運算；因為二維數組用得較多，而且基本可取代矩陣。
```py
 >>> a = np.mat([[1, 2, 3], [4, 5, 6]])   #使用mat函數創建一個2X3矩陣
 >>> a
 matrix([[1, 2, 3],
 [4, 5, 6]])
 >>> b = np.matrix([[1, 2, 3], [4, 5, 6]])#np.mat和np.matrix等價
 >>> b
 matrix([[1, 2, 3],
 [4, 5, 6]])
 >>> a.shape     #使用shape屬性可以獲取矩陣的大小
(2, 3)
```

```py
>>> c = np.array([[1, 2, 3], [4, 5, 6]]) #使用二維數組代替矩陣，常見的操作通用
>>> c#注意c是array類型，而a是matrix類型
array([[1, 2, 3],
[4, 5, 6]])
```
###單位陣的創建

```py
>>> I = np.eye(3)
>>> I
array([[ 1.,  0.,  0.],
       [ 0.,  1.,  0.],
       [ 0.,  0.,  1.]])
```

###矩陣元素的存取操作：

```py
>>> a[0]#獲取矩陣的某一行
matrix([[1, 2, 3]])
>>> a[:, 0].reshape(-1, 1)#獲取矩陣的某一列
matrix([[1],
        [4]])
>>> a[0, 1]#獲取矩陣某個元素
2
```

##二、矩陣乘法和加法
矩陣類型，在滿足乘法規則的條件下可以直接相乘

```py
>>> A = np.mat([[1, 2, 3], [3, 4, 5], [6, 7, 8]])#使用mat函數
>>> B = np.mat([[5, 4, 2], [1, 7, 9], [0, 4, 5]])
>>> A   #注意A, B都是matrix類型，可以使用乘號，如果是array則不可以直接使用乘號
matrix([[1, 2, 3],
        [3, 4, 5],
        [6, 7, 8]])
>>> B
matrix([[5, 4, 2],
        [1, 7, 9],
        [0, 4, 5]])
>>> A * B#學過線性代數的都知道：A * B != B * A
matrix([[  7,  30,  35],
        [ 19,  60,  67],
        [ 37, 105, 115]])
>>> B * A
matrix([[ 29,  40,  51],
        [ 76,  93, 110],
        [ 42,  51,  60]])
```

如果是使用數組代替矩陣進行運算則不可以直接使用乘號，應使用dot()函數。dot函數用於矩陣乘法，對於二維數組，它計算的是矩陣乘積，對於一維數組，它計算的是內積。

```py
>>> C = np.array([[1, 2, 3], [3, 4, 5], [6, 7, 8]])
>>> D = np.array([[5, 4, 2], [1, 7, 9], [0, 4, 5]])
>>> C          #C, D都是array類型，不能直接使用乘號，應該使用dot()函數
array([[1, 2, 3],
       [3, 4, 5],
       [6, 7, 8]])
>>> D
array([[5, 4, 2],
       [1, 7, 9],
       [0, 4, 5]])
#>>> C * D, Error, 注意這不是矩陣乘法！！！
>>> np.dot(C, D)#正確的寫法，得到的結果和上一段代碼的第11行的結果的一樣的。
array([[  7,  30,  35],
       [ 19,  60,  67],
       [ 37, 105, 115]])
```

如何理解對於一維數組，它計算的是內積？？？

注意：在線性代數裡面講的維數和數組的維數不同，如線代中提到的n維行向量在Python中是一維數組，而線代中的n維列向量在Python中是一個shape為(n, 1)的二維數組！

第16行，第18行：F是一維數組，G是二維數組，維數不同，個人認為相乘沒有意義，但是16行沒有錯誤，18行報錯。關於dot()的乘法規則見：NumPy-快速處理數據--矩陣運算

```py
>>> E = np.array([1, 2, 3])
>>> F = np.array([4, 3, 9])
>>> E.shape#E,F都是一維數組
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
>>> np.dot(F, G)#因此dot(F, G)不再是內積，而是一個只有一個元素的數組
array([106])
>>> np.dot(G, F)#ValueError: shapes (3,1) and (3,) not aligned: 1 (dim 1) != 3 (dim 0)
>>> E.shape = (1, -1)#把E改為二維數組
>>> E
array([[1, 2, 3]])
>>> E.shape
(1, 3)
>>> np.dot(G, E)#3×1的G向量乘以1×3的E向量會得到3×3的矩陣
array([[ 4,  8, 12],
       [ 3,  6,  9],
       [ 9, 18, 27]])
```

矩陣的加法運算

```py
>>> A + B#矩陣的加法對matrix類型和array類型是通用的
matrix([[ 6,  6,  5],
        [ 4, 11, 14],
        [ 6, 11, 13]])
>>> C + D
array([[ 6,  6,  5],
       [ 4, 11, 14],
       [ 6, 11, 13]])
```

矩陣的數乘運算
```py
>>> 2 * A#矩陣的數乘對matrix類型和array類型是通用的
matrix([[ 2,  4,  6],
        [ 6,  8, 10],
        [12, 14, 16]])
>>> 2 * C
array([[ 2,  4,  6],
       [ 6,  8, 10],
       [12, 14, 16]])
```

##三、矩陣的轉置

```py
>>> A = np.array([[1, 2, 3], [3, 4, 5], [6, 7, 8]])
>>> B = np.array([[5, 4, 2], [1, 7, 9], [0, 4, 5]])
>>> A
array([[1, 2, 3],
       [3, 4, 5],
       [6, 7, 8]])
>>> A.T  #A的轉置
array([[1, 3, 6],
       [2, 4, 7],
       [3, 5, 8]])
>>> A.T.T#A的轉置的轉置還是A本身
array([[1, 2, 3],
       [3, 4, 5],
       [6, 7, 8]])
```

驗證矩陣轉置的性質：(A±B)'=A'±B'

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

驗證矩陣轉置的性質：(KA)'=KA'

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

驗證矩陣轉置的性質：(A×B)'= B'×A'

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

##四、方陣的跡
方陣的跡就是主對角元素之和，使用trace()函數獲得方陣的跡：
```py
>>> A
array([[1, 2, 3],
       [3, 4, 5],
       [6, 7, 8]])
>>> B
array([[5, 4, 2],
       [1, 7, 9],
       [0, 4, 5]])
>>> np.trace(A)  # A的跡等於A.T的跡
13
>>> np.trace(A.T)
13
>>> np.trace(A+B)# 和的跡 等於 跡的和
30
>>> np.trace(A) + np.trace(B)
30
```

##五、計算行列式
```py
>>> A
array([[1, 2],
       [1, 3]])
>>> np.linalg.det(A)
1.0
```

##六、逆矩陣/伴隨矩陣
若A存在逆矩陣(滿足det(A) != 0，或者A滿秩)，使用linalg.inv求得方陣A的逆矩陣

```py
import numpy as np
>>> A = np.array([[1, -2, 1], [0, 2, -1], [1, 1, -2]])
>>> A
array([[ 1, -2,  1],
       [ 0,  2, -1],
       [ 1,  1, -2]])
>>> A_det = np.linalg.det(A)      #求A的行列式，不為零則存在逆矩陣
>>> A_det
-3.0000000000000004
>>> A_inverse = np.linalg.inv(A)  #求A的逆矩陣
>>> A_inverse
array([[ 1.        ,  1.        ,  0.        ],
       [ 0.33333333,  1.        , -0.33333333],
       [ 0.66666667,  1.        , -0.66666667]])
>>> np.dot(A, A_inverse)          #A與其逆矩陣的乘積為單位陣
array([[ 1.,  0.,  0.],
       [ 0.,  1.,  0.],
       [ 0.,  0.,  1.]])
>>> A_companion = A_inverse * A_det  #求A的伴隨矩陣
>>> A_companion
array([[-3., -3., -0.],
       [-1., -3.,  1.],
       [-2., -3.,  2.]])
```

##七、解一元線性方程
使用np.linalg.solve()解一元線性方程組，待解方程為：

```py
 x + 2y +  z = 7
2x -  y + 3z = 7
3x +  y + 2z =18
```

```py
>>> import numpy as np
>>> A = np.array([[1, 2, 1], [2, -1, 3], [3, 1, 2]])
>>> A    #係數矩陣
array([[ 1,  2,  1],
       [ 2, -1,  3],
       [ 3,  1,  2]])
>>> B = np.array([7, 7, 18])
>>> B
array([ 7,  7, 18])
>>> x = np.linalg.solve(A, B)
>>> x
array([ 7.,  1., -2.])
>>> np.dot(A, x)#檢驗正確性，結果為B
array([  7.,   7.,  18.])
```

使用np.allclose()檢測兩個矩陣是否相同：

```py
1 >>> np.allclose(np.dot(A, x), B)#檢驗正確性
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

##八、計算矩陣距離
矩陣的距離，這裡是的是歐幾里得距離，其他距離表示方法我們以後再談，這裡說一下如何計算兩個形狀相同矩陣之間的距離。

```py
>>> A = np.array([[0, 1], [1, 0]])#先創建兩個矩陣
>>> B = np.array([[1, 1], [1, 1]])
>>> C = A - B       #計算距離矩陣C
>>> C
array([[-1,  0],
       [ 0, -1]])
>>> D = np.dot(C, C)#距離矩陣的平方
>>> E = np.trace(D) #計算矩陣D的跡
>>> E
2
>>> E ** 0.5        #將E開平方得到距離
1.4142135623730951
```

關於計算矩陣距離我也不理解。網上看的帖子，先記下來

##九、矩陣的秩
numpy包中的linalg.matrix_rank方法計算矩陣的秩：

```py
>>> import numpy as np
>>> I = np.eye(3)#先創建一個單位陣
>>> I
array([[ 1.,  0.,  0.],
       [ 0.,  1.,  0.],
       [ 0.,  0.,  1.]])
>>> np.linalg.matrix_rank(I)#秩
3
>>> I[1, 1] = 0#將該元素置為0
>>> I
array([[ 1.,  0.,  0.],
       [ 0.,  0.,  0.],
       [ 0.,  0.,  1.]])
>>> np.linalg.matrix_rank(I)#此時秩變成2
2
```

##十、求方陣的特徵值特徵向量
```py 
>>> import numpy as np
>>> x = np.diag((1, 2, 3))#創建一個對角矩陣！
>>> x
array([[1, 0, 0],
       [0, 2, 0],
       [0, 0, 3]])
>>> a,b = np.linalg.eig(x)#特徵值保存在a中，特徵向量保存在b中
>>> a
array([ 1.,  2.,  3.])
>>> b
array([[ 1.,  0.,  0.],
       [ 0.,  1.,  0.],
       [ 0.,  0.,  1.]])
```

根據公式 Ax = λx 檢驗特徵值與特徵向量是否正確：

```py
for i in range(3):#方法一
    if np.allclose(np.dot(a[i], b[:, i]), x[:, i]):#np.allclose()方法在第七節提到過
        print 'Right'
    else:
        print 'Error'

for i in range(3):#方法二
    if (np.dot(a[i], b[:, i]) == x[:, i]).all():
        print 'Right'
    else:
        print 'Error'
```

注意，如果寫成 if np.dot(a[i], b[:, i]) == x[:, i]: 是錯誤的：(矩陣包含有多個值，應該使用a.any()或者a.all()判斷)

 ValueError: The truth value of an array with more than one element is ambiguous. Use a.any() or a.all() 

##十一、判斷正定矩陣
- 設M是n階方陣，如果對任何非零向量z，都有z'Mz> 0，其中z' 表示z的轉置，就稱M正定矩陣。

- 判定定理1：對稱陣A為正定的充分必要條件是：A的特徵值全為正。
- 判定定理2：對稱陣A為正定的充分必要條件是：A的各階順序主子式都為正。
- 判定定理3：任意陣A為正定的充分必要條件是：A合同於單位陣。

下面用定理1判斷對稱陣是否為正定陣

```py
>>> import numpy as np
>>> A = np.arange(16).reshape(4, 4)
>>> A
array([[ 0,  1,  2,  3],
       [ 4,  5,  6,  7],
       [ 8,  9, 10, 11],
       [12, 13, 14, 15]])
>>> A = A + A.T             #將方陣轉換成對稱陣
>>> A
array([[ 0,  5, 10, 15],
       [ 5, 10, 15, 20],
       [10, 15, 20, 25],
       [15, 20, 25, 30]])
>>> B = np.linalg.eigvals(A)#求B的特徵值，注意：eig()是求特徵值特徵向量
>>> B
array([  6.74165739e+01 +0.00000000e+00j,
        -7.41657387e+00 +0.00000000e+00j,
         2.04219701e-15 +3.94306094e-15j,
         2.04219701e-15 -3.94306094e-15j])

if np.all(B>0):             #判斷是不是所有的特徵值都大於0，用到了all函數，顯然對稱陣A不是正定的
    print 'Yes'
```

創建一個對角元素都為正的對角陣，它一定是正定的：

```py
>>> A = np.diag((1, 2, 3))#創建對角陣，其特徵值都為正
>>> B = np.linalg.eigvals(A)#求特徵值
>>> B
array([ 1.,  2.,  3.])
>>> if np.all(B>0):#判斷特徵值是否都大於0
    print 'Yes'
```

網上查到更簡便的方法是對對稱陣進行cholesky分解，如果像這樣沒有提示出錯，就說明它是正定的。如果提示出錯，就說明它不是正定矩陣，你可以使用try函數捕獲錯誤值：

```py
# -*- coding: utf-8 -*-
import numpy as np

A = np.arange(16).reshape(4, 4)
A = A + A.T
print A
try:
    B = np.linalg.cholesky(A)
except :
    print ('不是正定矩陣，不能進行cholesky分解。')
```

當不能進行cholesky分解時，出現的異常是： LinAlgError: Matrix is not positive definite ，但是但是LinAlgError不是Python標準異常，因此不能使用這條語句。

```py
1 except LinAlgError as reason:
2     print ('不是正定矩陣，不能進行cholesky分解。\n出錯原因是：' + str(reason))
```