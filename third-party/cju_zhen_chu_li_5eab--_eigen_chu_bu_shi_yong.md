# C++矩陣處理庫--Eigen初步使用



项目要进行比较多的矩阵操作，特别是二维矩阵。刚开始做实验时，使用了动态二维数组，于是写了一堆Matrix函数，作矩阵的乘除加减求逆求行列式。实验做完了，开始做代码优化，发现Matrix.h文件里适用性太低，而且动态二维数组的空间分配与释放也影响效率，于是寻找其他解决方案。

首先考虑的是与Matlab混合编程，折腾了半天把Matlab环境与VS2010环境之后，发现Matlab编译出来的函数使用起来也比较麻烦，要把数组转化成该函数适用的类型后才能使用这些函数。我的二维数组也不是上千万维的，估计这个转化的功夫就牺牲了一部分效率了。（如果谁有混合编程的心得，求帮忙，囧。。。）

接着想到使用一维数组的方法，或者把一维数组封装在一个类里边。想着又要写一堆矩阵操作函数头就大，索性谷歌了一下矩阵处理库，除了自己之前知道的OpenCV库（之前由于转化cvarr麻烦，于是放弃），还有Eigen, Armadillo。

http://blog.csdn.net/houston11235/article/details/8501135该博客对这三个库的效率做了一个简单的评测，OpenCV库的矩阵操作效率是最低的，还好我没使用。Eigen速度最快，与自己定义数组的操作效率相当（- -，才相当吗？我本来还想找个更快的呢）。于是选择使用Eigen。

进入正题。

安装：
http://eigen.tuxfamily.org/index.php?title=Main_Page这里是官网，直接把包下载下来，不大，也就几M，我是直接放在自己项目文件夹（考虑项目封装时，这样比较方便），放在VS2010 <INCLUDE>文件夹。

简单使用：
看了一下官方文档，Eigen库除了能实现各种矩阵操作外，貌似还提供《数学分析》中的各种矩阵操作（包括L矩阵U矩阵）。目前我使用到的还是简单的矩阵操作，如加减乘除，求行列式，转置，逆，这些基本操作只要:


```cpp
#include "Eigen/Eigen"  
using namespace Eigen;  
```

就能实现，别忘了名空间Eigen。
包含的类型：

```cpp
Matrices	Arrays
Matrix<float,Dynamic,Dynamic> <=> MatrixXf
Matrix<double,Dynamic,1> <=> VectorXd
Matrix<int,1,Dynamic> <=> RowVectorXi
Matrix<float,3,3> <=> Matrix3f
Matrix<float,4,1> <=> Vector4f
Array<float,Dynamic,Dynamic> <=> ArrayXXf
Array<double,Dynamic,1> <=> ArrayXd
Array<int,1,Dynamic> <=> RowArrayXi
Array<float,3,3> <=> Array33f
Array<float,4,1> <=> Array4f
```

如上表，主要包括两种类型，Matrices与Arryays，接着是这两种类型的派生类型。现在我用到的是Matrices（我不明白这两种类型在效率间有什么差距，囧。。。），其中Matrix代表二维矩阵，Vector代表列向量RowVector代表行向量。如果后面跟着X，则代表是动态的数组，运行时可以根据需求改变，如果是数字，则代表是静态的（根据实验，最多能建立4维的静态矩阵或者数组，- -，为嘛不是6维，实验正好需要）。i代表int类型，f代表float类型，d代表double。
对应关系：


<tbody>
<tr>
<td>Matrix</td>
<td>二维矩阵</td>
</tr>
<tr>
<td>Vector</td>
<td>列向量</td>
</tr>
<tr>
<td>RowVector</td>
<td>行向量</td>
</tr>
<tr>
<td>X</td>
<td>动态</td>
</tr>
<tr>
<td>固定数字n</td>
<td>静态，4&gt;=n&gt;=1</td>
</tr>
<tr>
<td>i</td>
<td>int</td>
</tr>
<tr>
<td>f</td>
<td>float</td>
</tr>
<tr>
<td>d</td>
<td>double</td>
</tr>
</tbody>


Arrays类型的话也跟Matrices差不多。
基本操作，定义，初始化，矩阵操作：


```cpp
#include <iostream>  
#include "Eigen/Eigen"  
using namespace std;  
using namespace Eigen;  
  
void foo(MatrixXf& m)  
{  
    Matrix3f m2=Matrix3f::Zero(3,3);  
    m2(0,0)=1;  
    m=m2;  
}  
int main()  
{  
    /* 定义，定义时默认没有初始化，必须自己初始化 */  
    MatrixXf m1(3,4);   //动态矩阵，建立3行4列。  
    MatrixXf m2(4,3);   //4行3列，依此类推。  
    MatrixXf m3(3,3);  
    Vector3f v1;        //若是静态数组，则不用指定行或者列  
    /* 初始化 */  
    m1 = MatrixXf::Zero(3,4);       //用0矩阵初始化,要指定行列数  
    m2 = MatrixXf::Zero(4,3);  
    m3 = MatrixXf::Identity(3,3);   //用单位矩阵初始化  
    v1 = Vector3f::Zero();          //同理，若是静态的，不用指定行列数  
  
    m1 << 1,0,0,1,        //也可以以这种方式初始化  
        1,5,0,1,  
        0,0,9,1;  
    m2 << 1,0,0,  
        0,4,0,  
        0,0,7,  
        1,1,1;  
      
    /* 元素的访问 */  
    v1[1] = 1;  
    m3(2,2) = 7;  
    cout<<"v1:\n"<<v1<<endl;  
    cout<<"m3:\n"<<m3<<endl;  
    /* 复制操作 */  
    VectorXf v2=v1;             //复制后，行数与列数和右边的v1相等,matrix也是一样,  
                                //也可以通过这种方式重置动态数组的行数与列数  
    cout<<"v2:\n"<<v2<<endl;  
  
    /* 矩阵操作，可以实现 + - * / 操作，同样可以实现连续操作(但是维数必须符合情况), 
    如m1,m2,m3维数相同,则可以m1 = m2 + m3 + m1; */  
    m3 = m1 * m2;  
    v2 += v1;  
    cout<<"m3:\n"<<m3<<endl;  
    cout<<"v2:\n"<<v2<<endl;  
    //m3 = m3.transpose();  这句出现错误，估计不能给自己赋值  
    cout<<"m3转置:\n"<<m3.transpose()<<endl;  
    cout<<"m3行列式:\n"<<m3.determinant()<<endl;  
    m3 = m3.inverse();  
    cout<<"m3求逆:\n"<<m3<<endl;  
  
  
    return 0;  
}  
```


