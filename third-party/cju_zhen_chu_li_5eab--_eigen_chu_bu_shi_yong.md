# C++矩陣處理庫--Eigen初步使用



項目要進行比較多的矩陣操作，特別是二維矩陣。剛開始做實驗時，使用了動態二維數組，於是寫了一堆Matrix函數，作矩陣的乘除加減求逆求行列式。實驗做完了，開始做代碼優化，發現Matrix.h文件裡適用性太低，而且動態二維數組的空間分配與釋放也影響效率，於是尋找其他解決方案。

首先考慮的是與Matlab混合編程，折騰了半天把Matlab環境與VS2010環境之後，發現Matlab編譯出來的函數使用起來也比較麻煩，要把數組轉化成該函數適用的類型後才能使用這些函數。我的二維數組也不是上千萬維的，估計這個轉化的功夫就犧牲了一部分效率了。（如果誰有混合編程的心得，求幫忙，囧。。。）

接著想到使用一維數組的方法，或者把一維數組封裝在一個類裡邊。想著又要寫一堆矩陣操作函數頭就大，索性谷歌了一下矩陣處理庫，除了自己之前知道的OpenCV庫（之前由於轉化cvarr麻煩，於是放棄），還有Eigen, Armadillo。

http://blog.csdn.net/houston11235/article/details/8501135該博客對這三個庫的效率做了一個簡單的評測，OpenCV庫的矩陣操作效率是最低的，還好我沒使用。Eigen速度最快，與自己定義數組的操作效率相當（- -，才相當嗎？我本來還想找個更快的呢）。於是選擇使用Eigen。

進入正題。

安裝：
http://eigen.tuxfamily.org/index.php?title=Main_Page這裡是官網，直接把包下載下來，不大，也就幾M，我是直接放在自己項目文件夾（考慮項目封裝時，這樣比較方便），放在VS2010 <INCLUDE>文件夾。

簡單使用：
看了一下官方文檔，Eigen庫除了能實現各種矩陣操作外，貌似還提供《數學分析》中的各種矩陣操作（包括L矩陣U矩陣）。目前我使用到的還是簡單的矩陣操作，如加減乘除，求行列式，轉置，逆，這些基本操作只要:


```cpp
#include "Eigen/Eigen"  
using namespace Eigen;  
```

就能實現，別忘了名空間Eigen。
包含的類型：

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

如上表，主要包括兩種類型，Matrices與Arryays，接著是這兩種類型的派生類型。現在我用到的是Matrices（我不明白這兩種類型在效率間有什麼差距，囧。。。），其中Matrix代表二維矩陣，Vector代表列向量RowVector代表行向量。如果後面跟著X，則代表是動態的數組，運行時可以根據需求改變，如果是數字，則代表是靜態的（根據實驗，最多能建立4維的靜態矩陣或者數組，- -，為嘛不是6維，實驗正好需要）。i代表int類型，f代表float類型，d代表double。
對應關係：


<tbody>
<tr>
<td>Matrix</td>
<td>二維矩陣</td>
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
<td>動態</td>
</tr>
<tr>
<td>固定數字n</td>
<td>靜態，4&gt;=n&gt;=1</td>
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


Arrays類型的話也跟Matrices差不多。
基本操作，定義，初始化，矩陣操作：


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
    /* 定義，定義時默認沒有初始化，必須自己初始化 */  
    MatrixXf m1(3,4);   //動態矩陣，建立3行4列。  
    MatrixXf m2(4,3);   //4行3列，依此類推。  
    MatrixXf m3(3,3);  
    Vector3f v1;        //若是靜態數組，則不用指定行或者列  
    /* 初始化 */  
    m1 = MatrixXf::Zero(3,4);       //用0矩陣初始化,要指定行列數  
    m2 = MatrixXf::Zero(4,3);  
    m3 = MatrixXf::Identity(3,3);   //用單位矩陣初始化  
    v1 = Vector3f::Zero();          //同理，若是靜態的，不用指定行列數  
  
    m1 << 1,0,0,1,        //也可以以這種方式初始化  
        1,5,0,1,  
        0,0,9,1;  
    m2 << 1,0,0,  
        0,4,0,  
        0,0,7,  
        1,1,1;  
      
    /* 元素的訪問 */  
    v1[1] = 1;  
    m3(2,2) = 7;  
    cout<<"v1:\n"<<v1<<endl;  
    cout<<"m3:\n"<<m3<<endl;  
    /* 複製操作 */  
    VectorXf v2=v1;             //複製後，行數與列數和右邊的v1相等,matrix也是一樣,  
                                //也可以通過這種方式重置動態數組的行數與列數  
    cout<<"v2:\n"<<v2<<endl;  
  
    /* 矩陣操作，可以實現 + - * / 操作，同樣可以實現連續操作(但是維數必須符合情況), 
    如m1,m2,m3維數相同,則可以m1 = m2 + m3 + m1; */  
    m3 = m1 * m2;  
    v2 += v1;  
    cout<<"m3:\n"<<m3<<endl;  
    cout<<"v2:\n"<<v2<<endl;  
    //m3 = m3.transpose();  這句出現錯誤，估計不能給自己賦值  
    cout<<"m3轉置:\n"<<m3.transpose()<<endl;  
    cout<<"m3行列式:\n"<<m3.determinant()<<endl;  
    m3 = m3.inverse();  
    cout<<"m3求逆:\n"<<m3<<endl;  
  
  
    return 0;  
}  
```


