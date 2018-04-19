#include <inc/utils/Matrix3x3d.h>
using namespace std;

const double EPSINON = 0.00001;

Matrix3x3d::Matrix3x3d()
{
    mPtr = mInstance;

    for (int i = 0; i < 9; i++) {
        mPtr[i] = 0.0f;
    }
}

Matrix3x3d::Matrix3x3d(const Matrix3x3d& other)
{
    mPtr = mInstance;

    for (int i = 0; i < 9; i++) {
        mPtr[i] = other.mPtr[i];
    }
}

Matrix3x3d::Matrix3x3d(double m00, double m01, double m02,
                       double m10, double m11, double m12, double m20,
                       double m21, double m22)
{
    mPtr = mInstance;

    mPtr[0] = m00;
    mPtr[1] = m01;
    mPtr[2] = m02;
    mPtr[3] = m10;
    mPtr[4] = m11;
    mPtr[5] = m12;
    mPtr[6] = m20;
    mPtr[7] = m21;
    mPtr[8] = m22;
}

Matrix3x3d::~Matrix3x3d()
{
}

void Matrix3x3d::setIdentity()
{
    mPtr[1] = mPtr[2] = mPtr[3] = mPtr[5] = mPtr[6] = mPtr[7] = 0.0f;
    mPtr[0] = mPtr[4] = mPtr[8] = 1.0f;
}

double Matrix3x3d::determinant() const
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

    //Reference http://www.mathsisfun.com/algebra/matrix-determinant.html
    double det = m00 * (m11 * m22 - m12 * m21) -
                 m01 * (m10 * m22 - m12 * m20) +
                 m02 * (m10 * m21 - m11 * m20);

    return det;
}

bool Matrix3x3d::inverse()
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

    double d = determinant();

    if ((d >= -EPSINON) && (d <= EPSINON)) {
        return false;
    }

    double invert_det = 1.0 / d;

    //Reference http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche23.html
    double inverse[9];
    inverse[0] = invert_det * (m11 * m22 - m12 * m21);
    inverse[1] = invert_det * (m02 * m21 - m01 * m22);
    inverse[2] = invert_det * (m01 * m12 - m02 * m11);

    inverse[3] = invert_det * (m12 * m20 - m10 * m22);
    inverse[4] = invert_det * (m00 * m22 - m02 * m20);
    inverse[5] = invert_det * (m02 * m10 - m00 * m12);

    inverse[6] = invert_det * (m10 * m21 - m11 * m20);
    inverse[7] = invert_det * (m01 * m20 - m00 * m21);
    inverse[8] = invert_det * (m00 * m11 - m01 * m10);

    for (int i = 0; i < 9; i++) {
        mPtr[i] = inverse[i];
    }

    return true;
}

void Matrix3x3d::transpose()
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

    mPtr[1] = m10;
    mPtr[2] = m20;

    mPtr[3] = m01;
    mPtr[5] = m21;

    mPtr[6] = m02;
    mPtr[7] = m12;
}

void Matrix3x3d::setSameDiagonal(double d)
{
    mPtr[0] = mPtr[4] = mPtr[8] = d;
}

bool Matrix3x3d::setRow(const Vector3d& v, unsigned int row)
{
    if (row >= 3) {
        return false;
    }

    mPtr[row * 3]     = v.getVal(0);
    mPtr[row * 3 + 1] = v.getVal(1);
    mPtr[row * 3 + 2] = v.getVal(2);
    return true;
}

bool Matrix3x3d::setColumn(const Vector3d& v, unsigned int col)
{
    if (col >= 3) {
        return false;
    }

    mPtr[col]     = v.getVal(0);
    mPtr[col + 3] = v.getVal(1);
    mPtr[col + 6] = v.getVal(2);
    return false;
}

void Matrix3x3d::scale(double s)
{
    for (int i = 0; i < 9; i++) {
        mPtr[i] *= s;
    }
}
