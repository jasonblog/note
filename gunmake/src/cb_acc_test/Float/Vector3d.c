#include "../Inc/float/Vector3d.h"
#define EPSILON 0.000001

#define max(a, b) (((a) > (b)) ? (a) : (b))

void new_Vector3d(Vector3d* this_)
{
    this_->x = 0.0;
    this_->y = 0.0;
    this_->z = 0.0;
}

void Vector3d_set(float xx, float yy, float zz, Vector3d* this_)
{
    /*TODO*/
    this_->x = xx;
    this_->y = yy;
    this_->z = zz;
}

void Vector3d_setComponent(int i, float val, Vector3d* this_)
{
    if (i == 0) {
        this_->x = val;
    } else if (i == 1) {
        this_->y = val;
    } else {
        this_->z = val;
    }
}

void Vector3d_setZero(Vector3d* this_)
{
    /*TODO*/
    this_->x = this_->y = this_->z = 0.0;
}

void Vector3d_set_o(Vector3d* other, Vector3d* this_)
{
    this_->x = other->x;
    this_->y = other->y;
    this_->z = other->z;
}

void Vector3d_scale(float s, Vector3d* this_)
{
    /*TODO*/
    this_->x *= s;
    this_->y *= s;
    this_->z *= s;
}

void Vector3d_normalize(Vector3d* this_)
{
    /*TODO*/
    float d = Vector3d_length(this_);

    if (fabs(d - 0) > EPSILON) {
        // if (d != 0.0) {
        Vector3d_scale(1.0 / d, this_);
    }
}

float Vector3d_dot(Vector3d* a, Vector3d* b)
{
    /*TODO*/
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

float Vector3d_length(Vector3d* this_)
{
    /*TODO*/
    return sqrt(this_->x * this_->x + this_->y * this_->y + this_->z * this_->z);
}

bool Vector3d_sameValues(Vector3d* other)
{
    return false;
}

void Vector3d_add(Vector3d* a, Vector3d* b, Vector3d* result)
{
    /*TODO*/
    Vector3d_set(a->x + b->x, a->y + b->y, a->z + b->z, result);
}

void Vector3d_sub(Vector3d* a, Vector3d* b, Vector3d* result)
{
    /*TODO*/
    Vector3d_set(a->x - b->x, a->y - b->y, a->z - b->z, result);
}

void Vector3d_cross(Vector3d* a, Vector3d* b, Vector3d* result)
{
    /*TODO*/
    Vector3d_set(a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z,
                 a->x * b->y - a->y * b->x, result);
}

void Vector3d_ortho(Vector3d* v, Vector3d* result)
{
    int k = Vector3d_largestAbsComponent(v) - 1;

    if (k < 0) {
        k = 2;
    }

    Vector3d_setZero(result);
    Vector3d_setComponent(k, 1.0, result);
    Vector3d_cross(v, result, result);
    Vector3d_normalize(result);
}

/*
string ToString()
{
}
*/

int Vector3d_largestAbsComponent(Vector3d* v)
{
    float xAbs = fabs(v->x);
    float yAbs = fabs(v->y);
    float zAbs = fabs(v->z);

    return xAbs > yAbs ? (xAbs > zAbs ? 0 : 2) : (yAbs > zAbs ? 1 : 2);
}

float Vector3d_maxNorm(Vector3d* this_)
{
    return max(fabs(this_->x), max(fabs(this_->y), fabs(this_->z)));
}
