#ifndef QUATERNION_H
#define QUATERNION_H

#include <iostream>

class Vector3d;
class RotationMatrix;

class Quaternion
{
public:
    Quaternion();
    Quaternion(double dW, double dX, double dY, double dZ);
    ~Quaternion();

    void normalize();
    void inverse();
    void converEulerTo(const Vector3d& angle);
    Vector3d toEuler() const ;
    RotationMatrix toRotationMatrix() const ;

    // inline operator overloading
    inline friend std::ostream& operator<< (std::ostream& stream,
                                            const Quaternion& q)
    {
        stream << "Q(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
        return stream;
    }

    double w;
    double x;
    double y;
    double z;
};

#endif // QUATERNION_H
