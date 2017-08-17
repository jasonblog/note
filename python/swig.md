# SWIG


- test.h

```c
//test.h
typedef int (*FUNC) (int, int);
int fn(int a, int b, int (*op)(int,int));
int add(int a, int b); 
int mul(int a, int b); 
int sub(int a, int b);
```

- test.c


```c
//test.c
#include "test.h"

int fn(int a, int b, FUNC op) 
{
    return op(a, b);
}

int add(int a, int b) 
{
    return a + b;
}

int mul(int a, int b) 
{
    return a * b;
}

int sub(int a, int b)
{
    return a - b;
}
```

- test.i

```c
//test.i
/*
%module test
%{
    int binary_op(int a, int b, int (*op)(int,int));
    int add(int,int);
    int sub(int,int);
    int mul(int,int);
%}

extern int binary_op(int, int , int (*op)(int,int));


%constant int add(int,int);
%constant int sub(int,int);
%constant int mul(int,int);
*/

%module test
%{
    #include "test.h"
%}

typedef int (*FUNC) (int, int);
extern int fn(int, int, FUNC);

%callback("%s_cb");
    extern int add(int a, int b);
    extern int mul(int a, int b);
    extern int sub(int a, int b);
%nocallback;
```

```sh
swig -python test.i
gcc -O2 -fPIC -c test.c test_wrap.c -I /usr/include/python2.7
gcc -shared test.o test_wrap.o -o _test.so


Python 2.6.6 (r266:84292, Dec 27 2010, 00:02:40)
[GCC 4.4.5] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> from test import *
>>> add(33,22)
55
```


---

##c++ swig

- TestModel.h

```cpp
#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <iostream>

enum RotSeq {zyx, zyz, zxy, zxz, yxz, yxy, yzx, yzy, xyz, xyx, xzy, xzx};

class Quaternion
{
public:
    Quaternion(): x(0), y(0), z(0), w(1) {};
    Quaternion(double x, double y, double z, double w): x(x), y(y), z(z), w(w) {};
    void twoaxisrot(double r11, double r12, double r21, double r31, double r32,
                    double res[]);
    void threeaxisrot(double r11, double r12, double r21, double r31, double r32,
                      double res[]);
    void quaternion2Euler(const Quaternion& q, double res[], RotSeq rotSeq);

    void normalize();
    double norm();
    static void Quat2Euler(double w, double x, double y, double z);

    friend Quaternion operator *(Quaternion& q1, Quaternion& q2);
    friend std::ostream& operator <<(std::ostream& stream, const Quaternion& q);

private:
    double x;
    double y;
    double z;
    double w;

};

#endif
```

- TestModel.cpp

```cpp
#include <iostream>
#include <cmath>
#include "TestModel.h"

void Quaternion::normalize()
{
    double norm = std::sqrt(x * x + y * y + z * z + w * w);
    x /= norm;
    y /= norm;
    z /= norm;
    w /= norm;
}

double Quaternion::norm()
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

double x;
double y;
double z;
double w;

void Quaternion::twoaxisrot(double r11, double r12, double r21, double r31,
                            double r32,
                            double res[])
{
    res[0] = atan2(r11, r12);
    res[1] = acos(r21);
    res[2] = atan2(r31, r32);
}

void Quaternion::threeaxisrot(double r11, double r12, double r21, double r31,
                              double r32,
                              double res[])
{
    res[0] = atan2(r31, r32);
    res[1] = asin(r21);
    res[2] = atan2(r11, r12);
}

// note:
// return values of res[] depends on rotSeq.
// i.e.
// for rotSeq zyx,
// x = res[0], y = res[1], z = res[2]
// for rotSeq xyz
// z = res[0], y = res[1], x = res[2]
// ...
void Quaternion::quaternion2Euler(const Quaternion& q, double res[],
                                  RotSeq rotSeq)
{
    switch (rotSeq) {
    case zyx:
        threeaxisrot(2 * (q.x * q.y + q.w * q.z),
                     q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                     -2 * (q.x * q.z - q.w * q.y),
                     2 * (q.y * q.z + q.w * q.x),
                     q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                     res);
        break;

    case zyz:
        twoaxisrot(2 * (q.y * q.z - q.w * q.x),
                   2 * (q.x * q.z + q.w * q.y),
                   q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                   2 * (q.y * q.z + q.w * q.x),
                   -2 * (q.x * q.z - q.w * q.y),
                   res);
        break;

    case zxy:
        threeaxisrot(-2 * (q.x * q.y - q.w * q.z),
                     q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                     2 * (q.y * q.z + q.w * q.x),
                     -2 * (q.x * q.z - q.w * q.y),
                     q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                     res);
        break;

    case zxz:
        twoaxisrot(2 * (q.x * q.z + q.w * q.y),
                   -2 * (q.y * q.z - q.w * q.x),
                   q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                   2 * (q.x * q.z - q.w * q.y),
                   2 * (q.y * q.z + q.w * q.x),
                   res);
        break;

    case yxz:
        threeaxisrot(2 * (q.x * q.z + q.w * q.y),
                     q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                     -2 * (q.y * q.z - q.w * q.x),
                     2 * (q.x * q.y + q.w * q.z),
                     q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                     res);
        break;

    case yxy:
        twoaxisrot(2 * (q.x * q.y - q.w * q.z),
                   2 * (q.y * q.z + q.w * q.x),
                   q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                   2 * (q.x * q.y + q.w * q.z),
                   -2 * (q.y * q.z - q.w * q.x),
                   res);
        break;

    case yzx:
        threeaxisrot(-2 * (q.x * q.z - q.w * q.y),
                     q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                     2 * (q.x * q.y + q.w * q.z),
                     -2 * (q.y * q.z - q.w * q.x),
                     q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                     res);
        break;

    case yzy:
        twoaxisrot(2 * (q.y * q.z + q.w * q.x),
                   -2 * (q.x * q.y - q.w * q.z),
                   q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                   2 * (q.y * q.z - q.w * q.x),
                   2 * (q.x * q.y + q.w * q.z),
                   res);
        break;

    case xyz:
        threeaxisrot(-2 * (q.y * q.z - q.w * q.x),
                     q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                     2 * (q.x * q.z + q.w * q.y),
                     -2 * (q.x * q.y - q.w * q.z),
                     q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                     res);
        break;

    case xyx:
        twoaxisrot(2 * (q.x * q.y + q.w * q.z),
                   -2 * (q.x * q.z - q.w * q.y),
                   q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                   2 * (q.x * q.y - q.w * q.z),
                   2 * (q.x * q.z + q.w * q.y),
                   res);
        break;

    case xzy:
        threeaxisrot(2 * (q.y * q.z + q.w * q.x),
                     q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z,
                     -2 * (q.x * q.y - q.w * q.z),
                     2 * (q.x * q.z + q.w * q.y),
                     q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                     res);
        break;

    case xzx:
        twoaxisrot(2 * (q.x * q.z - q.w * q.y),
                   2 * (q.x * q.y + q.w * q.z),
                   q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
                   2 * (q.x * q.z + q.w * q.y),
                   -2 * (q.x * q.y - q.w * q.z),
                   res);
        break;

    default:
        std::cout << "Unknown rotation sequence" << std::endl;
        break;
    }
}

///////////////////////////////
// Helper functions
///////////////////////////////
Quaternion operator*(Quaternion& q1, Quaternion& q2)
{
    Quaternion q;
    q.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    q.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    q.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    q.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return q;
}

std::ostream& operator<<(std::ostream& stream, const Quaternion& q)
{
    std::cout << q.w << " " << std::showpos << q.x << "i " << q.y << "j " << q.z <<
              "k";
    std::cout << std::noshowpos;
}

double rad2deg(double rad)
{
    return rad * 180.0 / M_PI;
}

void Quaternion::Quat2Euler(double w, double x, double y, double z)
{
    Quaternion q(x, y, z, w); // x,y,z,w
    double res[3];
    q.quaternion2Euler(q, res, static_cast<RotSeq>(zyx));
    std::cout << "z: " << rad2deg(res[0]) << " y: " << rad2deg(res[1]) << " x: " <<
              rad2deg(res[2]) << std::endl << std::endl;
}

#if 0
int main()
{
    Quaternion::Quat2Euler(-0.16151874, -0.042242825, -0.97542757, 0.14371127);
    return 0;
}
#endif
```

- TestModel.i

```cpp
%module TestModel
%{
#include "TestModel.h"
%}

enum RotSeq {zyx, zyz, zxy, zxz, yxz, yxy, yzx, yzy, xyz, xyx, xzy, xzx};

class Quaternion
{
public:
    Quaternion(): x(0), y(0), z(0), w(1) {};
    Quaternion(double x, double y, double z, double w): x(x), y(y), z(z), w(w) {};
    void twoaxisrot(double r11, double r12, double r21, double r31, double r32,
                    double res[]);
    void threeaxisrot(double r11, double r12, double r21, double r31, double r32,
                      double res[]);
    void quaternion2Euler(const Quaternion& q, double res[], RotSeq rotSeq);

    void normalize();
    double norm();
    static void Quat2Euler(double w, double x, double y, double z);

    friend Quaternion operator *(Quaternion& q1, Quaternion& q2);
    friend std::ostream& operator <<(std::ostream& stream, const Quaternion& q);

private:
    double x;
    double y;
    double z;
    double w;

};
```


```sh
swig -c++ -python TestModel.i
g++ -fPIC -c TestModel.cpp TestModel_wrap.cxx -I /usr/include/python2.7
g++ -shared TestModel.o TestModel_wrap.o -o _TestModel.so

from TestModel import *
Quaternion_Quat2Euler(-0.16151874, -0.042242825, -0.97542757, 0.14371127)
```
