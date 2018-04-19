#include <iostream>
#include <math.h>

#include <inc/utils/Quaternion.h>
#include <inc/utils/RotationMatrix.h>

using namespace std;

RotationMatrix::RotationMatrix()
    : Matrix3x3d()
{
}

RotationMatrix::RotationMatrix(double m00, double m01, double m02,
                               double m10, double m11, double m12, double m20, double m21, double m22)
    : Matrix3x3d(m00, m01, m02, m10, m11, m12, m20, m21, m22)
{
}

RotationMatrix::RotationMatrix(const RotationMatrix& m)
    : Matrix3x3d(m)
{
}

RotationMatrix::RotationMatrix(const Matrix3x3d& m)
    : Matrix3x3d(m)
{
}

RotationMatrix::~RotationMatrix()
{
}

Quaternion RotationMatrix::toQuat() const
{
    double m00 = mPtr[0];
    double m01 = mPtr[1];
    double m02 = mPtr[2];
    double m10 = mPtr[3];
    double m11 = mPtr[4];
    double m12 = mPtr[5];
    double m20 = mPtr[6];
    double m21 = mPtr[7];
    double m22 = mPtr[8];

    double qw, qx, qy, qz;

    double tr = m00 + m11 + m22;

    if (tr > 0) {
        double S = sqrt(tr + 1.0) * 2; // S=4*qw
        qw = 0.25 * S;
        qx = (m21 - m12) / S;
        qy = (m02 - m20) / S;
        qz = (m10 - m01) / S;
    } else if ((m00 > m11) & (m00 > m22)) {
        double S = sqrt(1.0 + m00 - m11 - m22) * 2; // S=4*qx
        qw = (m21 - m12) / S;
        qx = 0.25 * S;
        qy = (m01 + m10) / S;
        qz = (m02 + m20) / S;
    } else if (m11 > m22) {
        double S = sqrt(1.0 + m11 - m00 - m22) * 2; // S=4*qy
        qw = (m02 - m20) / S;
        qx = (m01 + m10) / S;
        qy = 0.25 * S;
        qz = (m12 + m21) / S;
    } else {
        double S = sqrt(1.0 + m22 - m00 - m11) * 2; // S=4*qz
        qw = (m10 - m01) / S;
        qx = (m02 + m20) / S;
        qy = (m12 + m21) / S;
        qz = 0.25 * S;
    }

    return Quaternion(qw, qx, qy, qz);

}

// RPY definition Reference http://www.euclideanspace.com/maths/geometry/rotations/euler/index.htm
// Calculation reference http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/
Vector3d RotationMatrix::toEuler() const
{
    double m00 = mPtr[0];
    double m01 = mPtr[1];
    double m02 = mPtr[2];
    double m10 = mPtr[3];
    double m11 = mPtr[4];
    double m12 = mPtr[5];
    double m20 = mPtr[6];
    double m21 = mPtr[7];
    double m22 = mPtr[8];

    double yaw;     //heading
    double pitch;   //attitude
    double roll;    //bank

    if (m10 > 0.998) {  //singularity at north pole
        yaw   = atan2(m02, m22);
        pitch = M_PI / 2;
        roll  = 0.0f;
    } else if (m10 < -0.998) {  //singularity at south pole
        yaw  = atan2(m02, m22);
        pitch = -M_PI / 2;
        roll  = 0.0f;
    } else {
        yaw   = atan2(-m20, m00);
        pitch = asin(m10);
        roll  = atan2(m12, m11);
    }

    return Vector3d(-roll, -yaw, -pitch);
}

RotationMatrix RotationMatrix::getRecenterMatrix() const
{
    RotationMatrix rm = *this;
    rm.inverse();
    return rm;
}

Vector3d RotationMatrix::getForwardVector() const
{
    return Vector3d(mPtr[6], mPtr[7], mPtr[8]);
}

Vector3d RotationMatrix::getRightVector() const
{
    return Vector3d(mPtr[0], mPtr[1], mPtr[2]);
}

Vector3d RotationMatrix::getUpVector() const
{
    return Vector3d(mPtr[3], mPtr[4], mPtr[5]);
}
