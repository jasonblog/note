#ifndef MATRIX3X3D_H
#define MATRIX3X3D_H
#include <iostream>
#include <inc/utils/Vector3d.h>

class Matrix3x3d
{
public:
    Matrix3x3d();
    Matrix3x3d(double m00, double m01, double m02,
               double m10, double m11, double m12,
               double m20, double m21, double m22);
    Matrix3x3d(const Matrix3x3d&);  //copy constructor
    ~Matrix3x3d();

    void setIdentity();
    double determinant() const;
    bool inverse();
    void transpose();
    void setSameDiagonal(double d);
    bool setRow(const Vector3d& v, unsigned int row);
    bool setColumn(const Vector3d& v, unsigned int col);
    void scale(double s);

    inline friend std::ostream& operator<< (std::ostream& stream,
                                            const Matrix3x3d& m)
    {
        stream << "[" << m.mInstance[0] << "|" << m.mInstance[1] << "|" <<
               m.mInstance[2] << "]";
        stream << "[" << m.mInstance[3] << "|" << m.mInstance[4] << "|" <<
               m.mInstance[5] << "]";
        stream << "[" << m.mInstance[6] << "|" << m.mInstance[7] << "|" <<
               m.mInstance[8] << "]";
        return stream;
    }

    inline Matrix3x3d& operator= (const Matrix3x3d& other)
    {
        if (this != &other) {
            mInstance[0] = other.mInstance[0];
            mInstance[1] = other.mInstance[1];
            mInstance[2] = other.mInstance[2];
            mInstance[3] = other.mInstance[3];
            mInstance[4] = other.mInstance[4];
            mInstance[5] = other.mInstance[5];
            mInstance[6] = other.mInstance[6];
            mInstance[7] = other.mInstance[7];
            mInstance[8] = other.mInstance[8];
            mPtr = &mInstance[0];
        }

        return *this;
    }

    inline Matrix3x3d& operator+= (const Matrix3x3d& rhs)
    {
        mInstance[0] += rhs.mInstance[0];
        mInstance[1] += rhs.mInstance[1];
        mInstance[2] += rhs.mInstance[2];
        mInstance[3] += rhs.mInstance[3];
        mInstance[4] += rhs.mInstance[4];
        mInstance[5] += rhs.mInstance[5];
        mInstance[6] += rhs.mInstance[6];
        mInstance[7] += rhs.mInstance[7];
        mInstance[8] += rhs.mInstance[8];
        return *this;
    }

    inline Matrix3x3d& operator *= (const Matrix3x3d& rhs)
    {
        Matrix3x3d lhs = *this;
        mInstance[0] = lhs.mPtr[0] * rhs.mPtr[0] + lhs.mPtr[1] * rhs.mPtr[3] +
                       lhs.mPtr[2] * rhs.mPtr[6];
        mInstance[1] = lhs.mPtr[0] * rhs.mPtr[1] + lhs.mPtr[1] * rhs.mPtr[4] +
                       lhs.mPtr[2] * rhs.mPtr[7];
        mInstance[2] = lhs.mPtr[0] * rhs.mPtr[2] + lhs.mPtr[1] * rhs.mPtr[5] +
                       lhs.mPtr[2] * rhs.mPtr[8];

        mInstance[3] = lhs.mPtr[3] * rhs.mPtr[0] + lhs.mPtr[4] * rhs.mPtr[3] +
                       lhs.mPtr[5] * rhs.mPtr[6];
        mInstance[4] = lhs.mPtr[3] * rhs.mPtr[1] + lhs.mPtr[4] * rhs.mPtr[4] +
                       lhs.mPtr[5] * rhs.mPtr[7];
        mInstance[5] = lhs.mPtr[3] * rhs.mPtr[2] + lhs.mPtr[4] * rhs.mPtr[5] +
                       lhs.mPtr[5] * rhs.mPtr[8];

        mInstance[6] = lhs.mPtr[6] * rhs.mPtr[0] + lhs.mPtr[7] * rhs.mPtr[3] +
                       lhs.mPtr[8] * rhs.mPtr[6];
        mInstance[7] = lhs.mPtr[6] * rhs.mPtr[1] + lhs.mPtr[7] * rhs.mPtr[4] +
                       lhs.mPtr[8] * rhs.mPtr[7];
        mInstance[8] = lhs.mPtr[6] * rhs.mPtr[2] + lhs.mPtr[7] * rhs.mPtr[5] +
                       lhs.mPtr[8] * rhs.mPtr[8];
        return *this;
    }

    double* mPtr;
    double mInstance[9];
};

////////////////////////////////////////////////////////////////////
// Inline operator overload
////////////////////////////////////////////////////////////////////

inline Matrix3x3d operator+ (const Matrix3x3d& lhs, const Matrix3x3d& rhs)
{
    Matrix3x3d m = lhs;
    m += rhs;
    return m;
}

inline Matrix3x3d operator* (const Matrix3x3d& lhs, const Matrix3x3d& rhs)
{
    Matrix3x3d m = lhs;
    m *= rhs;
    return m;
}
#endif // MATRIX3X3D_H
