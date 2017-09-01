#ifndef ROTATION_MATRIX_H
#define ROTATION_MATRIX_H
#include <inc/utils/Matrix3x3d.h>

class Quaternion;
class Vector3d;

class RotationMatrix : public Matrix3x3d
{
public:
    RotationMatrix();
    RotationMatrix(const RotationMatrix&);
    RotationMatrix(const Matrix3x3d&);
    RotationMatrix(double m00, double m01, double m02,
                   double m10, double m11, double m12,
                   double m20, double m21, double m22);
    ~RotationMatrix();

    Quaternion toQuat() const ;
    Vector3d toEuler() const ;
    RotationMatrix getRecenterMatrix() const;
    Vector3d getForwardVector() const;
    Vector3d getRightVector() const;
    Vector3d getUpVector() const;
};

#endif // ROTATION_MATRIX_H
