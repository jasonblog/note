#ifndef MATRIX_H
#define MATRIX_H
#if 1
void setIdentityM(float* sm, int smOffset);

void multiplyMM(float* result, int resultOffset,
                float* lhs, int lhsOffset,
                float* rhs, int rhsOffset);

void translateM(float* m, int mOffset,
                float x, float y, float z);

void translateM_l(float* tm, int tmOffset,
                  float* m, int mOffset,
                  float x, float y, float z);

void setRotateEulerM(float* rm, int rmOffset,
                     float x, float y, float z);
#else
void multiplyMM(float*, float*, float*);
void translateMM(float*, float*,  float, float, float);
void scaleMM(float*, float*, float, float, float);
void rotateMM(float*, float*, float, float, float, float);
void transposeMM(float*, float*);
void setIdentityM(float*);
void rotateM(float*, float, float, float, float);
void scaleM(float*, float, float, float);
void translateM(float*, float, float, float);
void transposeM(float*);
#endif
#endif /* MATRIX_H */
