# Eigen 使用


```sh
wget http://bitbucket.org/eigen/eigen/get/3.3.4.tar.bz2
```


```sh
tar xvf 3.3.4.tar.bz2
```

- add export  xxx in ~/.bashrc file

```sh
export CPLUS_INCLUDE_PATH=$PWD/eigen-eigen-5a0156e40feb/
```


```cpp
#include <Eigen/Dense>
#include <Eigen/Geometry>

using namespace Eigen;

/// Determine rotation quaternion from coordinate system 1 (vectors
/// x1, y1, z1) to coordinate system 2 (vectors x2, y2, z2)
Quaterniond QuaternionRot(Vector3d x1, Vector3d y1, Vector3d z1,
                          Vector3d x2, Vector3d y2, Vector3d z2) {

    Matrix3d M = x1*x2.transpose() + y1*y2.transpose() + z1*z2.transpose();

    Matrix4d N;
    N << M(0,0)+M(1,1)+M(2,2)   ,M(1,2)-M(2,1)          , M(2,0)-M(0,2)         , M(0,1)-M(1,0),
         M(1,2)-M(2,1)          ,M(0,0)-M(1,1)-M(2,2)   , M(0,1)+M(1,0)         , M(2,0)+M(0,2),
         M(2,0)-M(0,2)          ,M(0,1)+M(1,0)          ,-M(0,0)+M(1,1)-M(2,2)  , M(1,2)+M(2,1),
         M(0,1)-M(1,0)          ,M(2,0)+M(0,2)          , M(1,2)+M(2,1)         ,-M(0,0)-M(1,1)+M(2,2);

    EigenSolver<Matrix4d> N_es(N);
    Vector4d::Index maxIndex;
    N_es.eigenvalues().real().maxCoeff(&maxIndex);

    Vector4d ev_max = N_es.eigenvectors().col(maxIndex).real();

    Quaterniond quat(ev_max(0), ev_max(1), ev_max(2), ev_max(3));
    quat.normalize();

    return quat;
}

int main(int argc, char *argv[])
{
    
    return 0;
}
```


## Eigen基本用法

```cpp
#include <iostream>
using namespace std;

#include <Eigen/Core>
#include <Eigen/Dense>

int main()
{
    // 矩阵类模板的前三个参数为：数据类型，行，列
    Eigen::Matrix< float, 2, 3 > matrix_23;
    // 填充数据
    matrix_23 << 1, 2, 3, 4, 5, 6;
    // 输出:
    // 1 2 3
    // 4 5 6
    cout << matrix_23 << endl;

    // 访问元素
    // 输出：
    // 1
    // 2
    for (int i = 0; i < 1; i++)
        for (int j = 0; j < 2; j++) {
            cout << matrix_23(i, j) << endl;
        }

    // Vector3d实质上是Eigen::Matrix< double, 3, 1 >
    Eigen::Vector3d v_3d;
    v_3d << 3, 2, 1;

    // 矩阵和向量相乘，但不允许混合类型
    Eigen::Matrix< double, 2, 1 > result = matrix_23.cast< double >() * v_3d;
    // 输出：
    // 10
    // 28
    cout << result << endl;

    Eigen::Matrix3d matrix_33 = Eigen::Matrix3d::Zero();
    matrix_33 = Eigen::Matrix3d::Random();
    cout << matrix_33 << endl;

    cout << matrix_33.transpose() << endl;
    cout << matrix_33.sum() << endl;   // 各元素和
    cout << matrix_33.trace() << endl;
    cout << 10 * matrix_33 << endl;  // 数乘
    cout << matrix_33.inverse() << endl;   // 逆
    // cout << matrix_33.derterminant() << endl;

    // 特征值
    Eigen::SelfAdjointEigenSolver< Eigen::Matrix3d > eigen_solver(matrix_33);
    cout << "Eigen values = " << eigen_solver.eigenvalues() << endl;
    cout << "Eigen vectors = " << eigen_solver.eigenvectors() << endl;

    // 解方程
    // matrix_33 * x = v_3d
    // 直接求逆（运算量大）
    Eigen::Vector3d x = matrix_33.inverse() * v_3d;
    cout << "x = " << x << endl;
    // 矩阵分解法，如QR分解
    x = matrix_33.colPivHouseholderQr().solve(v_3d);
    cout << "x = " << x << endl;

    // 若不确定矩阵大小，可以使用动态大小的矩阵
    Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic > matrix_dynamic;

    Eigen::MatrixXd matrix_x;
    return 0;
}
```

