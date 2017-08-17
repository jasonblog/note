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
