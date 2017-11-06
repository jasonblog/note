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