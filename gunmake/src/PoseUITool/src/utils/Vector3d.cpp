#include <inc/utils/Vector3d.h>
#include <math.h>
#include <sstream>
#include <iostream>

#define max(a, b) (((a) > (b)) ? (a) : (b))
using namespace std;

Vector3d::Vector3d()
    : x(0), y(0), z(0)
{
    //cout << this << " ctor\n";
}

Vector3d::Vector3d(double xval, double yval, double zval)
    : x(xval), y(yval), z(zval)
{
    //cout << this << " ctor\n";
}

Vector3d::~Vector3d()
{
    //cout << this << " dtor\n";
}

void Vector3d::setComponent(int i, double val)
{
    if (i == 0) {
        x = val;
    } else if (i == 1) {
        y = val;
    } else {
        z = val;
    }
}

void Vector3d::setZero()
{
    x = y = z = 0.0;
}

void Vector3d::scale(double s)
{
    x *= s;
    y *= s;
    z *= s;
}

void Vector3d::normalize()
{
    double d = length();

    if (d != 0.0) {
        scale(1.0 / d);
    }
}

double Vector3d::dot(const Vector3d& a, const Vector3d& b) const
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double Vector3d::length() const
{
    return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

double Vector3d::getVal(int idx) const
{
    if (idx == 0) {
        return x;
    } else if (idx == 1) {
        return y;
    } else if (idx == 2) {
        return z;
    }

    return 0.0f;
}
