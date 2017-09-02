#include "../Inc/float/Matrix3x3d.h"
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define EPSILON 0.000001

void new_Matrix3x3d(Matrix3x3d* this_)
{
    int i;

    for (i = 0; i < 9; ++i) {
        this_->Instance_m[i] = 0.0;
    }

    this_->m = this_->Instance_m;
}

void Matrix3x3d_add(Matrix3x3d* a, Matrix3x3d* b,
                    Matrix3x3d* result)
{
    /*TODO*/
    result->m[0] = a->m[0] + b->m[0];
    result->m[1] = a->m[1] + b->m[1];
    result->m[2] = a->m[2] + b->m[2];
    result->m[3] = a->m[3] + b->m[3];
    result->m[4] = a->m[4] + b->m[4];
    result->m[5] = a->m[5] + b->m[5];
    result->m[6] = a->m[6] + b->m[6];
    result->m[7] = a->m[7] + b->m[7];
    result->m[8] = a->m[8] + b->m[8];
}

void Matrix3x3d_mult_M(Matrix3x3d* a, Matrix3x3d* b, Matrix3x3d* result)
{
    Matrix3x3d_set_m(a->m[0] * b->m[0] + a->m[1] * b->m[3] + a->m[2] * b->m[6],
                     a->m[0] * b->m[1] + a->m[1] * b->m[4] + a->m[2] * b->m[7],
                     a->m[0] * b->m[2] + a->m[1] * b->m[5] + a->m[2] * b->m[8],
                     a->m[3] * b->m[0] + a->m[4] * b->m[3] + a->m[5] * b->m[6],
                     a->m[3] * b->m[1] + a->m[4] * b->m[4] + a->m[5] * b->m[7],
                     a->m[3] * b->m[2] + a->m[4] * b->m[5] + a->m[5] * b->m[8],
                     a->m[6] * b->m[0] + a->m[7] * b->m[3] + a->m[8] * b->m[6],
                     a->m[6] * b->m[1] + a->m[7] * b->m[4] + a->m[8] * b->m[7],
                     a->m[6] * b->m[2] + a->m[7] * b->m[5] + a->m[8] * b->m[8],
                     result);
}

void Matrix3x3d_mult_V(Matrix3x3d* a, Vector3d* v, Vector3d* result)
{
    /*TODO*/
    float x = a->m[0] * v->x + a->m[1] * v->y + a->m[2] * v->z;
    float y = a->m[3] * v->x + a->m[4] * v->y + a->m[5] * v->z;
    float z = a->m[6] * v->x + a->m[7] * v->y + a->m[8] * v->z;

    result->x = x;
    result->y = y;
    result->z = z;
}

void Matrix3x3d_set_m(float m00, float m01, float m02,
                      float m10, float m11, float m12,
                      float m20, float m21, float m22,
                      Matrix3x3d* this_)
{
    /*TODO*/
    this_->m[0] = m00;
    this_->m[1] = m01;
    this_->m[2] = m02;
    this_->m[3] = m10;
    this_->m[4] = m11;
    this_->m[5] = m12;
    this_->m[6] = m20;
    this_->m[7] = m21;
    this_->m[8] = m22;
}

void Matrix3x3d_set_o(Matrix3x3d* o, Matrix3x3d* this_)
{
    /*TODO*/
    this_->m[0] = o->m[0];
    this_->m[1] = o->m[1];
    this_->m[2] = o->m[2];
    this_->m[3] = o->m[3];
    this_->m[4] = o->m[4];
    this_->m[5] = o->m[5];
    this_->m[6] = o->m[6];
    this_->m[7] = o->m[7];
    this_->m[8] = o->m[8];
}

void Matrix3x3d_setZero(Matrix3x3d* this_)
{
    this_->m[0] = this_->m[1] = this_->m[2] = this_->m[3] =
                                    this_->m[4] = this_->m[5] = this_->m[6] = this_->m[7] = this_->m[8] = 0.0;
}

void Matrix3x3d_setIdentity(Matrix3x3d* this_)
{
    /*TODO*/
    this_->m[1] = this_->m[2] = this_->m[3] = this_->m[5] = this_->m[6] =
                                    this_->m[7] = 0.0;
    this_->m[0] = this_->m[4] = this_->m[8] = 1.0;
}

void Matrix3x3d_setSameDiagonal(float d, Matrix3x3d* this_)
{
    this_->m[0] = this_->m[4] = this_->m[8] = d;
}

float Matrix3x3d_get(int row, int col, Matrix3x3d* this_)
{
    /*TODO*/
    return this_->m[3 * row + col];
}

void Matrix3x3d_set_v(int row, int col, float value, Matrix3x3d* this_)
{
    this_->m[3 * row + col] = value;
}

void Matrix3x3d_getColumn(int col, Vector3d* v, Matrix3x3d* this_)
{
    /*TODO*/
    v->x = this_->m[col];
    v->y = this_->m[col + 3];
    v->z = this_->m[col + 6];
}

void Matrix3x3d_setColumn(int col, Vector3d* v, Matrix3x3d* this_)
{
    /*TODO*/
    this_->m[col] = v->x;
    this_->m[col + 3] = v->y;
    this_->m[col + 6] = v->z;
}

void Matrix3x3d_scale(float s, Matrix3x3d* this_)
{
    /*TODO*/
    this_->m[0] *= s;
    this_->m[1] *= s;
    this_->m[2] *= s;
    this_->m[3] *= s;
    this_->m[4] *= s;
    this_->m[5] *= s;
    this_->m[6] *= s;
    this_->m[7] *= s;
    this_->m[8] *= s;
}

void Matrix3x3d_plusEquals(Matrix3x3d* b, Matrix3x3d* this_)
{
    /*TODO*/
    this_->m[0] += b->m[0];
    this_->m[1] += b->m[1];
    this_->m[2] += b->m[2];
    this_->m[3] += b->m[3];
    this_->m[4] += b->m[4];
    this_->m[5] += b->m[5];
    this_->m[6] += b->m[6];
    this_->m[7] += b->m[7];
    this_->m[8] += b->m[8];
}

void Matrix3x3d_minusEquals(Matrix3x3d* b, Matrix3x3d* this_)
{
    /*TODO*/
    this_->m[0] -= b->m[0];
    this_->m[1] -= b->m[1];
    this_->m[2] -= b->m[2];
    this_->m[3] -= b->m[3];
    this_->m[4] -= b->m[4];
    this_->m[5] -= b->m[5];
    this_->m[6] -= b->m[6];
    this_->m[7] -= b->m[7];
    this_->m[8] -= b->m[8];
}

void Matrix3x3d_transpose(Matrix3x3d* this_)
{
    /*TODO*/
    float tmp = this_->m[1];
    this_->m[1] = this_->m[3];
    this_->m[3] = tmp;
    tmp = this_->m[2];
    this_->m[2] = this_->m[6];
    this_->m[6] = tmp;
    tmp = this_->m[5];
    this_->m[5] = this_->m[7];
    this_->m[7] = tmp;
}

void Matrix3x3d_transpose_r(Matrix3x3d* result, Matrix3x3d* this_)
{
    float m1 = this_->m[1];
    float m2 = this_->m[2];
    float m5 = this_->m[5];

    result->m[0] = this_->m[0];
    result->m[1] = this_->m[3];
    result->m[2] = this_->m[6];
    result->m[3] = m1;
    result->m[4] = this_->m[4];
    result->m[5] = this_->m[7];
    result->m[6] = m2;
    result->m[7] = m5;
    result->m[8] = this_->m[8];
}

float Matrix3x3d_determinant(Matrix3x3d* this_)
{
    /*TODO*/
    return Matrix3x3d_get(0, 0, this_) * (Matrix3x3d_get(1, 1, this_) *
                                          Matrix3x3d_get(2, 2, this_) - Matrix3x3d_get(2, 1, this_) *
                                          Matrix3x3d_get(1, 2, this_)) -
           Matrix3x3d_get(0, 1, this_) * (Matrix3x3d_get(1, 0, this_) * Matrix3x3d_get(2,
                                          2, this_) -
                                          Matrix3x3d_get(1, 2, this_) * Matrix3x3d_get(2, 0, this_)) +
           Matrix3x3d_get(0, 2, this_) *
           (Matrix3x3d_get(1, 0, this_) *
            Matrix3x3d_get(2, 1, this_) - Matrix3x3d_get(1, 1, this_) *
            Matrix3x3d_get(2, 0, this_));
}

bool Matrix3x3d_invert(Matrix3x3d* result, Matrix3x3d* this_)
{
    /*TODO*/
    float d = Matrix3x3d_determinant(this_);

    // if (d == 0.0) {
    if (fabs(d - 0) < EPSILON) {
        return false;
    } else {
        float invdet = 1.0 / d;

        Matrix3x3d_set_m((this_->m[4] * this_->m[8] - this_->m[7] * this_->m[5]) *
                         invdet,
                         -(this_->m[1] * this_->m[8] - this_->m[2] * this_->m[7]) * invdet,
                         (this_->m[1] * this_->m[5] - this_->m[2] * this_->m[4]) * invdet,
                         -(this_->m[3] * this_->m[8] - this_->m[5] * this_->m[6]) * invdet,
                         (this_->m[0] * this_->m[8] - this_->m[2] * this_->m[6]) * invdet,
                         -(this_->m[0] * this_->m[5] - this_->m[3] * this_->m[2]) * invdet,
                         (this_->m[3] * this_->m[7] - this_->m[6] * this_->m[4]) * invdet,
                         -(this_->m[0] * this_->m[7] - this_->m[6] * this_->m[1]) * invdet,
                         (this_->m[0] * this_->m[4] - this_->m[3] * this_->m[1]) * invdet,
                         result);

        return true;
    }
}

float Matrix3x3d_maxNorm(Matrix3x3d* this_, int length)
{
    int i;

    float maxVal = fabs(this_->m[0]);

    for (i = 1; i < length; ++i) {
        maxVal = max(maxVal, fabs(this_->m[i]));
    }

    return maxVal;
}
