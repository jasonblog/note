# NumPy中array和matrix用于矩阵乘法时的区别


在NumPy中，array用于表示通用的N维数组，matrix则特定用于线性代数计算。array和matrix都可以用来表示矩阵，二者在进行乘法操作时，有一些不同之处。

　　使用array时，运算符 * 用于计算数量积（点乘），函数 dot() 用于计算矢量积（叉乘），例子如：