#include <math.h>
#include "../Inc/float/Matrix.h"

#define M_PI  3.14159265358979323846 /* pi */

/**
 * Sets matrix m to the identity matrix.
 *
 * @param sm returns the result
 * @param smOffset index into sm where the result matrix starts
 */
void setIdentityM(float* sm, int smOffset)
{
    int i;

    for (i = 0 ; i < 16 ; i++) {
        sm[smOffset + i] = 0;
    }

    for (i = 0; i < 16; i += 5) {
        sm[smOffset + i] = 1.0f;
    }
}

#if 1
#define I(_i, _j) ((_j)+ 4*(_i))
void  multiplyMM(float* result, int resultOffset,
                 float* lhs, int lhsOffset,
                 float* rhs, int rhsOffset)
{
    float* result1 = result + resultOffset;
    float* lhs1 = lhs + lhsOffset;
    float* rhs1 = rhs + rhsOffset;

    for (int i = 0 ; i < 4 ; i++) {
        const float rhs_i0 = rhs1[ I(i, 0) ];
        float ri0 = lhs1[ I(0, 0) ] * rhs_i0;
        float ri1 = lhs1[ I(0, 1) ] * rhs_i0;
        float ri2 = lhs1[ I(0, 2) ] * rhs_i0;
        float ri3 = lhs1[ I(0, 3) ] * rhs_i0;

        for (int j = 1 ; j < 4 ; j++) {
            const float rhs_ij = rhs1[ I(i, j) ];
            ri0 += lhs1[ I(j, 0) ] * rhs_ij;
            ri1 += lhs1[ I(j, 1) ] * rhs_ij;
            ri2 += lhs1[ I(j, 2) ] * rhs_ij;
            ri3 += lhs1[ I(j, 3) ] * rhs_ij;
        }

        result1[ I(i, 0) ] = ri0;
        result1[ I(i, 1) ] = ri1;
        result1[ I(i, 2) ] = ri2;
        result1[ I(i, 3) ] = ri3;
    }
}
#else
#define I(_i, _j) ((_j)+ 4*(_i))
void multiplyMM(float* r, const float* lhs, const float* rhs)
{
    int i, j;

    for (i = 0 ; i < 4 ; i++) {
        const float rhs_i0 = rhs[ I(i, 0) ];
        float ri0 = lhs[ I(0, 0) ] * rhs_i0;
        float ri1 = lhs[ I(0, 1) ] * rhs_i0;
        float ri2 = lhs[ I(0, 2) ] * rhs_i0;
        float ri3 = lhs[ I(0, 3) ] * rhs_i0;

        for (j = 1 ; j < 4 ; j++) {
            const float rhs_ij = rhs[ I(i, j) ];
            ri0 += lhs[ I(j, 0) ] * rhs_ij;
            ri1 += lhs[ I(j, 1) ] * rhs_ij;
            ri2 += lhs[ I(j, 2) ] * rhs_ij;
            ri3 += lhs[ I(j, 3) ] * rhs_ij;
        }

        r[ I(i, 0) ] = ri0;
        r[ I(i, 1) ] = ri1;
        r[ I(i, 2) ] = ri2;
        r[ I(i, 3) ] = ri3;
    }
}
#endif

/**
 * Translates matrix m by x, y, and z in place.
 *
 * @param m matrix
 * @param mOffset index into m where the matrix starts
 * @param x translation factor x
 * @param y translation factor y
 * @param z translation factor z
 */
void translateM(float* m, int mOffset,
                float x, float y, float z)
{
    int i;

    for (i = 0 ; i < 4 ; i++) {
        int mi = mOffset + i;
        m[12 + mi] += m[mi] * x + m[4 + mi] * y + m[8 + mi] * z;
    }
}

/**
 * Translates matrix m by x, y, and z, putting the result in tm.
 * <p>
 * m and tm must not overlap.
 *
 * @param tm returns the result
 * @param tmOffset index into sm where the result matrix starts
 * @param m source matrix
 * @param mOffset index into m where the source matrix starts
 * @param x translation factor x
 * @param y translation factor y
 * @param z translation factor z
 */
void translateM_l(float* tm, int tmOffset,
                  float* m, int mOffset,
                  float x, float y, float z)
{
    int i;

    for (i = 0 ; i < 12 ; i++) {
        tm[tmOffset + i] = m[mOffset + i];
    }

    for (i = 0 ; i < 4 ; i++) {
        int tmi = tmOffset + i;
        int mi = mOffset + i;
        tm[12 + tmi] = m[mi] * x + m[4 + mi] * y + m[8 + mi] * z +
                       m[12 + mi];
    }
}

/**
 * Converts Euler angles to a rotation matrix.
 *
 * @param rm returns the result
 * @param rmOffset index into rm where the result matrix starts
 * @param x angle of rotation, in degrees
 * @param y angle of rotation, in degrees
 * @param z angle of rotation, in degrees
 */
void setRotateEulerM(float* rm, int rmOffset, float x, float y, float z)
{
    float cx;
    float sx;
    float cy;
    float sy;
    float cz;
    float sz;
    float cxsy;
    float sxsy;

    x *= (float)(M_PI / 180.0f);
    y *= (float)(M_PI / 180.0f);
    z *= (float)(M_PI / 180.0f);

    cx = (float) cos(x);
    sx = (float) sin(x);
    cy = (float) cos(y);
    sy = (float) sin(y);
    cz = (float) cos(z);
    sz = (float) sin(z);
    cxsy = cx * sy;
    sxsy = sx * sy;

    rm[rmOffset + 0]  =   cy * cz;
    rm[rmOffset + 1]  =  -cy * sz;
    rm[rmOffset + 2]  =   sy;
    rm[rmOffset + 3]  =  0.0f;

    rm[rmOffset + 4]  =  cxsy * cz + cx * sz;
    rm[rmOffset + 5]  = -cxsy * sz + cx * cz;
    rm[rmOffset + 6]  =  -sx * cy;
    rm[rmOffset + 7]  =  0.0f;

    rm[rmOffset + 8]  = -sxsy * cz + sx * sz;
    rm[rmOffset + 9]  =  sxsy * sz + sx * cz;
    rm[rmOffset + 10] =  cx * cy;
    rm[rmOffset + 11] =  0.0f;

    rm[rmOffset + 12] =  0.0f;
    rm[rmOffset + 13] =  0.0f;
    rm[rmOffset + 14] =  0.0f;
    rm[rmOffset + 15] =  1.0f;
}

#if 0
#include "../include/Matrix.h"
#include <math.h>
#include <string.h>

#define M_PI (3.14159265358979323846)
#define to_rad(x) ((float)((x * M_PI) / 180.f))
/** @file */
/**
    @brief Multiplies two matrices and stores the result in a matrix
    @param result Where the result will be stored
    @param L the left hand side of the operation
    @param R the right hand side of the opertation

*/
void multiplyMM(float* result, float* L, float* R)
{
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            result[r * 4 + c] = L[c + 0 * 4] * R[r * 4 + 0] +
                                L[c + 1 * 4] * R[r * 4 + 1] +
                                L[c + 2 * 4] * R[r * 4 + 2] +
                                L[c + 3 * 4] * R[r * 4 + 3];

    return;
}
/**
    @brief Translates the matrix m and stores it in matrix R
    @param r the result matrix
    @param m the target matrix
    @param x the x translation
    @param y the y translation
    @param z the z translation
*/
void translateMM(float* r, float* m, float x, float y, float z)
{
    float translationMatrix[16] = {1, 0, 0, 0,
                                   0, 1, 0, 0,
                                   0, 0, 1, 0,
                                   x, y, z, 1
                                  };
    multiplyMM(r, m, translationMatrix);
}
/**
    @brief scales the matrix m and stores it in r
    @param r the result matrix
    @param m the target matrix
    @param sx the scale factor for the X dimension
    @param sy the scale factor for the Y dimension
    @param sz the scale factor for the Z dimension
*/
void scaleMM(float* r, float* m, float sx, float sy, float sz)
{
    float scaleMatrix[16] = {sx, 0 , 0 , 0,
                             0 , sy, 0 , 0,
                             0 , 0 , sz, 0,
                             0 , 0 , 0 , 1
                            };
    multiplyMM(r, m, scaleMatrix);
}
/**
    @brief performs a axis angle rotation around the arbitrary axis (_x,_y,_z)
    @param r the result matrix
    @param m the target matrix
    @param angle the angle (in degrees) to rotate
    @param _x the x component of the vector to use as a axis
    @param _y the y component of the vector to use as a axis
    @param _z the z component of the vector to use as a axis
*/
void rotateMM(float* r, float* m, float angle, float _x, float _y, float _z)
{
    float s = sin(to_rad(angle));
    float c = cos(to_rad(angle));
    float magnitude = sqrt(_x * _x + _y * _y + _z * _z);
    float x = _x / magnitude;
    float y = _y / magnitude;
    float z = _z / magnitude;
    float rotationMatrix[16] = {c + (x * x)* (1 - c), y* x* (1 - c) + z * s, z* x* (1 - c) - y * s, 0,
                                y* x* (1 - c) - z * s, c + (y * y)* (1 - c), z* y* (1 - c) + x * s, 0,
                                z* x* (1 - c) + y * s, y* z* (1 - c) - x * s, c + z* z* (1 - c)    , 0,
                                0, 0, 0, 1
                               };

    multiplyMM(r, m, rotationMatrix);
    return;
}
/**
    @brief transposes the matrix `matrix` and stores the result in `result`
    @param result the result matrix
    @param matrix the matrix to transpose
*/
void transposeMM(float* result, float* matrix)
{
    for (int c = 0; c < 4; c++) /* HA */
        for (int r = 0; r < 4; r++) {
            result[c * 4 + r] = matrix[r * 4 + c];
        }
}
/**
    @brief sets the identity matrix for m
    @param m the matrix to be set to a identity matrix
*/
void setIdentityM(float* m)
{
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++) {
            m[y * 4 + x] = x == y ? 1 : 0;
        }
}
/**
    @brief rotates the matrix mat, angle degrees around the arbitrary axis (x,y,z)
    @param mat the matrix to be rotated
    @param angle the angle (in degrees) to rotate the matrix
    @param x the x component of the vector to use as an axis
    @param y the y component of the vector to use as an axis
    @param z the z component of the vector to use as an axis
*/
void rotateM(float* mat, float angle, float x, float y, float z)
{
    float result[16];
    rotateMM(result, mat, angle, x, y, z);
    memcpy(mat, result, sizeof(float) * 16);
}

/**
    @brief scales the matrix mat by sx, sy, and sz on the x, y, and z axis, respectively
    @param mat the matrix to scale
    @param sx the x scale component
    @param sy the y scale component
    @param sz the z scale component
*/

void scaleM(float* mat, float sx, float sy, float sz)
{
    float result[16];
    scaleMM(result, mat, sx, sy, sz);
    memcpy(mat, result, sizeof(float) * 16);
}

/**
    @brief translates the matrix mat, by x, y, and z on the x, y, and z axis', respectively
    @param mat the matrix to translate
    @param x the x translation component
    @param y the y translation component
    @param z the z translation component
*/
void translateM(float* mat, float x, float y, float z)
{
    float result[16];
    translateMM(result, mat, x, y, z);
    memcpy(mat, result, sizeof(float) * 16);
}

/**
    @brief transposes the matrix mat
    @param mat the matrix to transpose
*/
void transposeM(float* mat)
{
    float result[16];
    transposeMM(result, mat);
    memcpy(mat, result, sizeof(float) * 16);
}
#endif
