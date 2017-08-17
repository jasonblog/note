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
    static void Quat2Euler(double w, double x, double y, double z, RotSeq rotSeq);

    friend Quaternion operator *(Quaternion& q1, Quaternion& q2);
    friend std::ostream& operator <<(std::ostream& stream, const Quaternion& q);

private:
    double x;
    double y;
    double z;
    double w;

};

#endif
