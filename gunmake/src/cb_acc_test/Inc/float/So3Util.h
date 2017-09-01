#ifndef SO3UTIL_H
#define SO3UTIL_H
#include "Vector3d.h"
#include "Matrix3x3d.h"

void Init_So3Util();
void So3Util_sO3FromTwoVec(Vector3d* a, Vector3d* b, Matrix3x3d* result);
void So3Util_sO3FromMu(Vector3d* w, Matrix3x3d* result);
void So3Util_muFromSO3(Matrix3x3d* so3, Vector3d* result);
void So3Util_generatorField(int i, Matrix3x3d* pos, Matrix3x3d* result);
#endif /* SO3UTIL_H */
