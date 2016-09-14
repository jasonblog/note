# Quat  Euler 互轉


```cpp

// COMPILE: g++ -o quat2EulerTest quat2EulerTest.cpp
#include <iostream>
#include <cmath>

using namespace std;

///////////////////////////////
// Quaternion struct
// Simple incomplete quaternion struct for demo purpose
///////////////////////////////
struct Quaternion {
    Quaternion(): x(0), y(0), z(0), w(1) {};
    Quaternion(double x, double y, double z, double w): x(x), y(y), z(z), w(w) {};

    void normalize()
    {
        double norm = std::sqrt(x * x + y * y + z * z + w * w);
        x /= norm;
        y /= norm;
        z /= norm;
        w /= norm;
    }

    double norm()
    {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    double x;
    double y;
    double z;
    double w;
};

struct Euler {
    Euler(): x(0), y(0), z(0) {};
    Euler(double x, double y, double z): x(x), y(y), z(z) {};

    double x;
    double y;
    double z;
};

///////////////////
// Helper functions
///////////////////////////////

ostream& operator <<(std::ostream& stream, const Quaternion& q)
{
    stream << q.w << " " << showpos << q.x << "i " << q.y << "j " << q.z << "k";
    stream << noshowpos;
    return stream;
}

double rad2deg(double rad)
{
    return rad * 180.0 / M_PI;
}

double deg2rad(double deg)
{
    return deg * M_PI / 180.0;
}

Quaternion operator*(Quaternion& q1, Quaternion& q2)
{
    Quaternion q;
    q.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    q.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    q.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    q.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return q;
}

///////////////////////////////
// Quaternion to Euler
///////////////////////////////
enum RotSeq {zyx, zyz, zxy, zxz, yxz, yxy, yzx, yzy, xyz, xyx, xzy, xzx};

void twoaxisrot(double r11, double r12, double r21, double r31, double r32,
                double res[])
{
    res[0] = atan2(r11, r12);
    res[1] = acos(r21);
    res[2] = atan2(r31, r32);
}

void threeaxisrot(double r11, double r12, double r21, double r31, double r32,
                  double res[])
{
    res[0] = atan2(r31, r32);
    res[1] = asin(r21);
    res[2] = atan2(r11, r12);
}

void Euler2quaternion(const Euler& e, Quaternion& q, RotSeq rotSeq)
{
    // x, y, z, w
    Quaternion qx(sin(deg2rad(e.x) / 2), 0, 0, cos(deg2rad(e.x) / 2.0));
    Quaternion qy(0, sin(deg2rad(e.y) / 2.0),  0, cos(deg2rad(e.y) / 2.0));
    Quaternion qz(0, 0, sin(deg2rad(e.z) / 2.0), cos(deg2rad(e.z) / 2.0));
    Quaternion q1;

    switch (rotSeq) {
    case xyz:
        q1 = qy * qz;
        q = qx * q1;
        break;

    case xzy:
        q1 = qz * qy;
        q = qx * q1;
        break;

    case yxz:
        q1 = qx * qz;
        q = qy * q1;
        break;

    case yzx:
        q1 = qz * qx;
        q = qy * q1;
        break;

    case zxy:
        q1 = qx * qy;
        q = qz * q1;
        break;

    case zyx:
        q1 = qy * qx;
        q = qz * q1;
        break;

    default:
        std::cout << "Unknown rotation sequence" << std::endl;
        break;
    }
}

void quaternion2Euler(const Quaternion& q, double res[], RotSeq rotSeq)
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
// Main
///////////////////////////////
int main()
{
#if 1
    double res[3];
    Quaternion q;

    Euler e(45, 45, 0);
    Euler2quaternion(e, q, zyx);

    cout << "quat w = "  << q.w << endl;
    cout << "quat x = "  << q.x << endl;
    cout << "quat y = "  << q.y << endl;
    cout << "quat z = "  << q.z << endl;

    quaternion2Euler(q, res, zyx);

    printf("euler x=%f\n", rad2deg(res[0]));
    printf("euler y=%f\n", rad2deg(res[1]));
    printf("euler z=%f\n", rad2deg(res[2]));

#else
    Quaternion q; // x,y,z,w
    Quaternion qx45(sin(M_PI / 8), 0, 0, cos(M_PI / 8));
    Quaternion qy45(0, sin(M_PI / 8), 0, cos(M_PI / 8));
    Quaternion qz45(0, 0, sin(M_PI / 8), cos(M_PI / 8));
    Quaternion qx90(sin(M_PI / 4), 0, 0, cos(M_PI / 4));
    Quaternion qy90(0, sin(M_PI / 4), 0, cos(M_PI / 4));
    Quaternion qz90(0, 0, sin(M_PI / 4), cos(M_PI / 4));

    double res[3];

    q = qz45 * qx45;
    q.normalize();
    quaternion2Euler(q, res, zyx);
    cout << "Rotation sequence: X->Y->Z" << endl;
    cout << "x45 -> z45" << endl;
    cout << "q: " << q << endl;
    cout << "x: " << rad2deg(res[0]) << " y: " << rad2deg(res[1]) << " z: " <<
         rad2deg(res[2]) << endl << endl;

    q = qz90 * qx90;
    q.normalize();
    quaternion2Euler(q, res, zyx);
    cout << "Rotation sequence: X->Y->Z" << endl;
    cout << "x90 -> z90" << endl;
    cout << "q: " << q << endl;
    cout << "x: " << rad2deg(res[0]) << " y: " << rad2deg(res[1]) << " z: " <<
         rad2deg(res[2]) << endl << endl;

    q = qx90 * qz90;
    q.normalize();
    quaternion2Euler(q, res, xyz);
    cout << "Rotation sequence: Z->Y->X" << endl;
    cout << "z90 -> x90" << endl;
    cout << "q: " << q << endl;
    cout << "x: " << rad2deg(res[0]) << " y: " << rad2deg(res[1]) << " z: " <<
         rad2deg(res[2]) << endl;
#endif
}

```