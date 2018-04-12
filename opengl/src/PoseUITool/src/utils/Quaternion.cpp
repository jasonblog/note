#include <iostream>
#include <math.h>

#include <inc/utils/Quaternion.h>
#include <inc/utils/RotationMatrix.h>
#include <inc/utils/Vector3d.h>

using namespace std;
const double EPSINON = 0.00001;

Quaternion::Quaternion()
    : w(1.0f), x(0.0f), y(0.0f), z(0.0f)
{
}

Quaternion::Quaternion(double dW, double dX, double dY, double dZ)
    : w(dW), x(dX), y(dY), z(dZ)
{
}

Quaternion::~Quaternion()
{
}

void Quaternion::normalize()
{
    double n =
        sqrt(pow(w, 2) + pow(x, 2) + pow(y, 2) + pow(z, 2));
    w /= n;
    x /= n;
    y /= n;
    z /= n;
}

void Quaternion::inverse()
{
    double d = pow(w, 2) + pow(x, 2) + pow(y, 2) + pow(z, 2);

    w = w / d;
    x = -x / d;
    y = -y / d;
    z = -z / d;
}

void Quaternion::converEulerTo(const Vector3d& angle)
{
    Vector3d radius(angle.getVal(0) * M_PI / 180.0,
                    angle.getVal(1) * M_PI / 180.0, angle.getVal(2) * M_PI / 180.0);
    //cout << "radius: " << radius << endl;
    // (bank, heading, attitude)
    // (roll, yaw, pitch)

#if 0
    double c1 = cos(angle.getVal(0));
    double s1 = sin(angle.getVal(0));
    double c2 = cos(angle.getVal(1));
    double s2 = sin(angle.getVal(1));
    double c3 = cos(angle.getVal(2));
    double s3 = sin(angle.getVal(2));

    this->w = sqrt(1.0 + c1 * c2 + c1 * c3 - s1 * s2 * s3 * c2 * c3) / 2.0f;
    double w4 = 4.0f * this->w;
    this->x = (c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4;
    this->y = (s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4;
    this->z = (-s1 * s3 + c1 * s2 * c3 + s2) / w4;
#else
    double c1 = cos(radius.getVal(1) / 2);
    double s1 = sin(radius.getVal(1) / 2);
    double c2 = cos(radius.getVal(2) / 2);
    double s2 = sin(radius.getVal(2) / 2);
    double c3 = cos(radius.getVal(0) / 2);
    double s3 = sin(radius.getVal(0) / 2);

    w = c1 * c2 * c3 - s1 * s2 * s3;
    x = s1 * s2 * c3 + c1 * c2 * s3;
    y = s1 * c2 * c3 + c1 * s2 * s3;
    z = c1 * s2 * c3 - s1 * c2 * s3;
#endif
    normalize();
}


RotationMatrix Quaternion::toRotationMatrix() const
{
    double m00, m01, m02, m10, m11, m12, m20, m21, m22;
    double sqw = pow(w, 2);
    double sqx = pow(x, 2);
    double sqy = pow(y, 2);
    double sqz = pow(z, 2);

    // invs (inverse square length) is only required if quaternion is not already normalised
    double invs = 1 / (sqx + sqy + sqz + sqw);
    m00 = (sqx - sqy - sqz + sqw) * invs ;
    m11 = (-sqx + sqy - sqz + sqw) * invs ;
    m22 = (-sqx - sqy + sqz + sqw) * invs ;

    double tmp1 = x * y;
    double tmp2 = z * w;
    m10 = 2.0 * (tmp1 + tmp2) * invs ;
    m01 = 2.0 * (tmp1 - tmp2) * invs ;

    tmp1 = x * z;
    tmp2 = y * w;
    m20 = 2.0 * (tmp1 - tmp2) * invs ;
    m02 = 2.0 * (tmp1 + tmp2) * invs ;

    tmp1 = y * z;
    tmp2 = x * w;
    m21 = 2.0 * (tmp1 + tmp2) * invs ;
    m12 = 2.0 * (tmp1 - tmp2) * invs ;
    return RotationMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);
}

Vector3d Quaternion::toEuler() const
{
    double yaw;     //heading
    double pitch;   //attitude
    double roll;    //bank
    // Calculation reference http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/
    // with normalize quaternion
    double sqw  = pow(w, 2);
    double sqx  = pow(x, 2);
    double sqy  = pow(y, 2);
    double sqz  = pow(z, 2);
    double unit = sqw + sqx + sqy +
                  sqz; // if normalised is one, otherwise is correction factor
    double test = x * y + z * w;

#if 0

    if (test > 0.4999 * unit) {    // singularity at north pole
        cout << test << " > 0.4999\n";
        roll  = 0;
        yaw   = 2 * atan2(x, w);
        pitch = M_PI / 2.0f;
        return Vector3d(-roll, -yaw, -pitch);
    }

    if (test < -0.4999 * unit) {   // singularity at south pole
        cout << test << " < 0.4999\n";
        roll  = 0.0f;
        yaw   = -2 * atan2(x, w);
        pitch = -M_PI / 2.0f;
        return Vector3d(-roll, -yaw, -pitch);
    }

#endif

    roll  = atan2(2 * x * w - 2 * y * z, -sqx + sqy - sqz + sqw);   //bank
    yaw   = atan2(2 * y * w - 2 * x * z, sqx - sqy - sqz + sqw);    //heading
    pitch = asin(2 * test / unit); //attitude

    return Vector3d(roll, yaw, pitch);
}
