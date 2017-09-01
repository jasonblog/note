#ifndef MATRIX3X3D_H
#define MATRIX3X3D_H
#include "../base.h"
#include "Vector3d.h"

typedef struct Matrix3x3d_ {
    float* m;
    float Instance_m[9];
} Matrix3x3d;

void new_Matrix3x3d(Matrix3x3d* this_);
void Matrix3x3d_add(Matrix3x3d* a, Matrix3x3d* b,
                    Matrix3x3d* result);

void Matrix3x3d_mult_M(Matrix3x3d* a, Matrix3x3d* b, Matrix3x3d* result);

void Matrix3x3d_mult_V(Matrix3x3d* a, Vector3d* v,
                       Vector3d* result);


void Matrix3x3d_set_m(float m00, float m01, float m02,
                      float m10, float m11, float m12,
                      float m20, float m21, float m22,
                      Matrix3x3d* this_);

void Matrix3x3d_set_o(Matrix3x3d* o, Matrix3x3d* this_);
void Matrix3x3d_setZero(Matrix3x3d* this_);
void Matrix3x3d_setIdentity(Matrix3x3d* this_);
void Matrix3x3d_setSameDiagonal(float d, Matrix3x3d* this_);
float Matrix3x3d_get(int row, int col, Matrix3x3d* this_);
void Matrix3x3d_set_v(int row, int col, float value, Matrix3x3d* this_);
void Matrix3x3d_getColumn(int col, Vector3d* v, Matrix3x3d* this_);
void Matrix3x3d_setColumn(int col, Vector3d* v, Matrix3x3d* this_);
void Matrix3x3d_scale(float s, Matrix3x3d* this_);
void Matrix3x3d_plusEquals(Matrix3x3d* b, Matrix3x3d* this_);
void Matrix3x3d_minusEquals(Matrix3x3d* b, Matrix3x3d* this_);
void Matrix3x3d_transpose(Matrix3x3d* this_);
void Matrix3x3d_transpose_r(Matrix3x3d* result, Matrix3x3d* this_);
float Matrix3x3d_determinant(Matrix3x3d* this_);
bool Matrix3x3d_invert(Matrix3x3d* result, Matrix3x3d* this_);
float Matrix3x3d_maxNorm(Matrix3x3d* this_, int length);
#endif  /* MATRIX3X3D_H */
