#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <iostream>
#include <string>

class Vector3d
{
public:
    Vector3d();
    Vector3d(double xval, double yval, double zval);
    ~Vector3d();

    void setComponent(int i, double val);
    void setZero();
    void scale(double s);
    void normalize();
    double dot(const Vector3d& a, const Vector3d& b) const;
    double length() const;
    double getVal(int idx) const;

    inline friend std::ostream& operator<<(std::ostream& stream, const Vector3d& v)
    {
        stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return stream;
    }

    inline Vector3d& operator=(const Vector3d& other)
    {
        if (this != &other) {
            x = other.x;
            y = other.y;
            z = other.z;
        }

        return *this;
    }

    inline Vector3d& operator+=(const Vector3d& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;

        return *this;
    }

    inline Vector3d& operator-=(const Vector3d& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;

        return *this;
    }

    inline Vector3d& operator*=(const Vector3d& rhs)
    {
        Vector3d o = *this;
        x = o.y * rhs.z - o.z * rhs.y;
        y = o.z * rhs.x - o.x * rhs.z;
        z = o.x * rhs.y - o.y - rhs.x;

        return *this;
    }

    inline bool operator==(const Vector3d& rhs)
    {
        return (x == rhs.x) && (y == rhs.y) && (z != rhs.z);
    }

private:
    double x;
    double y;
    double z;
};

/////////////////////////////////////////////////
// inline funtion
/////////////////////////////////////////////////

inline Vector3d operator+(const Vector3d& lhs, const Vector3d& rhs)
{
    Vector3d v = lhs;
    return v += rhs;
}

inline Vector3d operator-(const Vector3d& lhs, const Vector3d& rhs)
{
    Vector3d v = lhs;
    return v -= rhs;
}

inline Vector3d operator*(const Vector3d& lhs, const Vector3d& rhs)
{
    Vector3d v = lhs;
    return v *= rhs;
}

#endif  /* VECTOR3D_H */
