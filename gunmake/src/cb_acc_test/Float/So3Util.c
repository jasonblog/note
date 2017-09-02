#include "../Inc/float/So3Util.h"
#define EPSILON 0.000001

static const float M_SQRT1_2_1 = 0.7071067811865476;
static const float ONE_6TH = 0.1666666716337204;
static const float ONE_20TH = 0.1666666716337204;

typedef struct So3Util_ {
    Vector3d* temp31;
    Vector3d Instance_temp31;
    Vector3d* sO3FromTwoVecN;
    Vector3d Instance_sO3FromTwoVecN;
    Vector3d* sO3FromTwoVecA;
    Vector3d Instance_sO3FromTwoVecA;
    Vector3d* sO3FromTwoVecB;
    Vector3d Instance_sO3FromTwoVecB;
    Vector3d* sO3FromTwoVecRotationAxis;
    Vector3d Instance_sO3FromTwoVecRotationAxis;
    Matrix3x3d* sO3FromTwoVec33R1;
    Matrix3x3d Instance_sO3FromTwoVec33R1;
    Matrix3x3d* sO3FromTwoVec33R2;
    Matrix3x3d Instance_sO3FromTwoVec33R2;
    Vector3d* muFromSO3R2;
    Vector3d Instance_muFromSO3R2;
    Vector3d* rotationPiAboutAxisTemp;
    Vector3d Instance_rotationPiAboutAxisTemp;
} So3Util;

static So3Util Instance_So3Util;

static void So3Util_rodriguesSo3Exp(Vector3d* w, float kA, float kB,
                                    Matrix3x3d* result);

static void So3Util_rotationPiAboutAxis(Vector3d* v, Matrix3x3d* result);

void Init_So3Util()
{
    Instance_So3Util.temp31 = &Instance_So3Util.Instance_temp31;
    new_Vector3d(Instance_So3Util.temp31);

    Instance_So3Util.sO3FromTwoVecN = &Instance_So3Util.Instance_sO3FromTwoVecN;
    new_Vector3d(Instance_So3Util.sO3FromTwoVecN);

    Instance_So3Util.sO3FromTwoVecA = &Instance_So3Util.Instance_sO3FromTwoVecA;
    new_Vector3d(Instance_So3Util.sO3FromTwoVecA);

    Instance_So3Util.sO3FromTwoVecB = &Instance_So3Util.Instance_sO3FromTwoVecB;
    new_Vector3d(Instance_So3Util.sO3FromTwoVecB);

    Instance_So3Util.sO3FromTwoVecRotationAxis =
        &Instance_So3Util.Instance_sO3FromTwoVecRotationAxis;
    new_Vector3d(Instance_So3Util.sO3FromTwoVecRotationAxis);

    Instance_So3Util.sO3FromTwoVec33R1 =
        &Instance_So3Util.Instance_sO3FromTwoVec33R1;
    new_Matrix3x3d(Instance_So3Util.sO3FromTwoVec33R1);

    Instance_So3Util.sO3FromTwoVec33R2 =
        &Instance_So3Util.Instance_sO3FromTwoVec33R2;
    new_Matrix3x3d(Instance_So3Util.sO3FromTwoVec33R2);

    Instance_So3Util.muFromSO3R2 = &Instance_So3Util.Instance_muFromSO3R2;
    new_Vector3d(Instance_So3Util.muFromSO3R2);

    Instance_So3Util.rotationPiAboutAxisTemp =
        &Instance_So3Util.Instance_rotationPiAboutAxisTemp;
    new_Vector3d(Instance_So3Util.rotationPiAboutAxisTemp);
}

static void So3Util_rodriguesSo3Exp(Vector3d* w, float kA, float kB,
                                    Matrix3x3d* result)
{
    float a = w->x * w->x;
    float b = w->y * w->y;
    float wz2 = w->z * w->z;

    Matrix3x3d_set_v(0, 0, 1.0 - kB * (b + wz2), result);
    Matrix3x3d_set_v(1, 1, 1.0 - kB * (a + wz2), result);
    Matrix3x3d_set_v(2, 2, 1.0 - kB * (a + b), result);
    a = kA * w->z;
    b = kB * w->x * w->y;
    Matrix3x3d_set_v(0, 1, b - a, result);
    Matrix3x3d_set_v(1, 0, b + a, result);
    a = kA * w->y;
    b = kB * w->x * w->z;
    Matrix3x3d_set_v(0, 2, b + a, result);
    Matrix3x3d_set_v(2, 0, b - a, result);
    a = kA * w->x;
    b = kB * w->y * w->z;
    Matrix3x3d_set_v(1, 2, b - a, result);
    Matrix3x3d_set_v(2, 1, b + a, result);
}

static void So3Util_rotationPiAboutAxis(Vector3d* v, Matrix3x3d* result)
{
    Vector3d_set_o(v, Instance_So3Util.rotationPiAboutAxisTemp);
    Vector3d_scale(3.141592653589793 / Vector3d_length(
                       Instance_So3Util.rotationPiAboutAxisTemp),
                   Instance_So3Util.rotationPiAboutAxisTemp);

    float kA = 0.0;
    float kB = 0.20264236728467558;
    So3Util_rodriguesSo3Exp(Instance_So3Util.rotationPiAboutAxisTemp, kA, kB,
                            result);
}

void So3Util_sO3FromTwoVec(Vector3d* a, Vector3d* b, Matrix3x3d* result)
{
    Vector3d_cross(a, b, Instance_So3Util.sO3FromTwoVecN);

    // if (Vector3d_length(Instance_So3Util.sO3FromTwoVecN) == 0.0) {
    if (fabs(Vector3d_length(Instance_So3Util.sO3FromTwoVecN) - 0) < EPSILON) {
        double r11 = Vector3d_dot(a, b);

        if (r11 >= 0.0) {
            Matrix3x3d_setIdentity(result);
        } else {
            Vector3d_ortho(a, Instance_So3Util.sO3FromTwoVecRotationAxis);
            So3Util_rotationPiAboutAxis(Instance_So3Util.sO3FromTwoVecRotationAxis, result);
        }
    } else {
        Vector3d_set_o(a, Instance_So3Util.sO3FromTwoVecA);
        Vector3d_set_o(b, Instance_So3Util.sO3FromTwoVecB);
        Vector3d_normalize(Instance_So3Util.sO3FromTwoVecN);
        Vector3d_normalize(Instance_So3Util.sO3FromTwoVecA);
        Vector3d_normalize(Instance_So3Util.sO3FromTwoVecB);
        Matrix3x3d* r1 = Instance_So3Util.sO3FromTwoVec33R1;
        Matrix3x3d_setColumn(0, Instance_So3Util.sO3FromTwoVecA, r1);
        Matrix3x3d_setColumn(1, Instance_So3Util.sO3FromTwoVecN, r1);
        Vector3d_cross(Instance_So3Util.sO3FromTwoVecN, Instance_So3Util.sO3FromTwoVecA,
                       Instance_So3Util.temp31);
        Matrix3x3d_setColumn(2, Instance_So3Util.temp31, r1);

        Matrix3x3d* r2 = Instance_So3Util.sO3FromTwoVec33R2;
        Matrix3x3d_setColumn(0, Instance_So3Util.sO3FromTwoVecB, r2);
        Matrix3x3d_setColumn(1, Instance_So3Util.sO3FromTwoVecN, r2);
        Vector3d_cross(Instance_So3Util.sO3FromTwoVecN, Instance_So3Util.sO3FromTwoVecB,
                       Instance_So3Util.temp31);
        Matrix3x3d_setColumn(2, Instance_So3Util.temp31, r2);
        Matrix3x3d_transpose(r1);
        Matrix3x3d_mult_M(r2, r1, result);
    }
}

void So3Util_sO3FromMu(Vector3d* w, Matrix3x3d* result)
{
    float thetaSq = Vector3d_dot(w, w);
    float theta = sqrt(thetaSq);
    float kA;
    float kB;

    if (thetaSq < 1.0E-8) {
        kA = 1.0 - 0.1666666716337204 * thetaSq;
        kB = 0.5;
    } else if (thetaSq < 1.0E-6) {
        kB = 0.5 - 0.0416666679084301 * thetaSq;
        kA = 1.0 - thetaSq * 0.1666666716337204 * (1.0 - 0.1666666716337204 *
                thetaSq);
    } else {
        float invTheta = 1.0 / theta;
        kA = sin(theta) * invTheta;
        kB = (1.0 - cos(theta)) * invTheta * invTheta;
    }

    So3Util_rodriguesSo3Exp(w, kA, kB, result);
}

void So3Util_muFromSO3(Matrix3x3d* so3, Vector3d* result)
{
    float cosAngle = (Matrix3x3d_get(0, 0, so3) + Matrix3x3d_get(1, 1,
                      so3) + Matrix3x3d_get(2, 2, so3) - 1.0) * 0.5;
    Vector3d_set((Matrix3x3d_get(2, 1, so3) - Matrix3x3d_get(1, 2, so3)) / 2.0,
                 (Matrix3x3d_get(0, 2, so3) -
                  Matrix3x3d_get(2, 0, so3)) / 2.0, (Matrix3x3d_get(1, 0, so3) - Matrix3x3d_get(0,
                          1, so3)) / 2.0, result);
    float sinAngleAbs = Vector3d_length(result);

    if (cosAngle > 0.7071067811865476) {
        if (sinAngleAbs > 0.0) {
            Vector3d_scale(asin(sinAngleAbs) / sinAngleAbs, result);
        }
    } else {
        float angle;

        if (cosAngle > -0.7071067811865476) {
            angle = acos(cosAngle);
            Vector3d_scale(angle / sinAngleAbs, result);
        } else {

            angle = 3.141592653589793 - asin(sinAngleAbs);
            float d0 = Matrix3x3d_get(0, 0, so3) - cosAngle;
            float d1 = Matrix3x3d_get(1, 1, so3) - cosAngle;
            float d2 = Matrix3x3d_get(2, 2, so3) - cosAngle;
            Vector3d* r2 = Instance_So3Util.muFromSO3R2;

            if (d0 * d0 > d1 * d1 && d0 * d0 > d2 * d2) {
                Vector3d_set(d0, (Matrix3x3d_get(1, 0, so3) +
                                  Matrix3x3d_get(0, 1, so3)) / 2.0, (Matrix3x3d_get(0, 2, so3) + Matrix3x3d_get(2,
                                          0, so3)) / 2.0, r2);
            } else if (d1 * d1 > d2 * d2) {
                Vector3d_set((Matrix3x3d_get(1, 0, so3) +
                              Matrix3x3d_get(0, 1, so3)) / 2.0, d1, (Matrix3x3d_get(2, 1,
                                      so3) + Matrix3x3d_get(1, 2, so3)) / 2.0, r2);
            } else {
                Vector3d_set((Matrix3x3d_get(0, 2, so3) +
                              Matrix3x3d_get(2, 0, so3)) / 2.0, (Matrix3x3d_get(2, 1, so3) + Matrix3x3d_get(1,
                                      2, so3)) / 2.0, d2, r2);
            }

            if (Vector3d_dot(r2, result) < 0.0) {
                Vector3d_scale(-1.0, r2);
            }

            Vector3d_normalize(r2);
            Vector3d_scale(angle, r2);
            Vector3d_set_o(r2, result);
        }
    }
}

void So3Util_generatorField(int i, Matrix3x3d* pos, Matrix3x3d* result)
{
    Matrix3x3d_set_v(i, 0, 0.0, result);
    Matrix3x3d_set_v((i + 1) % 3, 0, -Matrix3x3d_get((i + 2) % 3, 0, pos), result);
    Matrix3x3d_set_v((i + 2) % 3, 0, Matrix3x3d_get((i + 1) % 3, 0, pos), result);
}
