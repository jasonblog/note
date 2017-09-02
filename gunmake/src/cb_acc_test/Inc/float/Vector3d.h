#ifndef VECTOR3D_H
#define VECTOR3D_H
#include <math.h>
#include "../base.h"

typedef struct Vector3d_ {
    float x;
    float y;
    float z;
} Vector3d;

void new_Vector3d(Vector3d* this_);
void Vector3d_set(float xx, float yy, float zz, Vector3d* this_);
void Vector3d_setComponent(int i, float val, Vector3d* this_);
void Vector3d_setZero(Vector3d* this_);
void Vector3d_set_o(Vector3d* other, Vector3d* this_);
void Vector3d_scale(float s, Vector3d* this_);
void Vector3d_normalize(Vector3d* this_);
float Vector3d_dot(Vector3d* a, Vector3d* b);
float Vector3d_length(Vector3d* this_);
bool Vector3d_sameValues(Vector3d* other);
void Vector3d_add(Vector3d* a, Vector3d* b, Vector3d* this_);
void Vector3d_sub(Vector3d* a, Vector3d* b, Vector3d* this_);
void Vector3d_cross(Vector3d* a, Vector3d* b, Vector3d* this_);
void Vector3d_ortho(Vector3d* v, Vector3d* this_);
/*string ToString();*/
int Vector3d_largestAbsComponent(Vector3d* v);
float Vector3d_maxNorm();
#endif  /* VECTOR3D_H */
