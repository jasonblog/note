#ifndef HEADTRANSFORM_H
#define HEADTRANSFORM_H

typedef struct HeadTransform_ {
    float* headView;
    float Instance_headView[16];
} HeadTransform;

void new_HeadTransform(HeadTransform* this_);

void arraycopy();
float* getHeadView_l(HeadTransform* this_);

void getHeadView(float headView[], int length,
                 int offset, HeadTransform* this_);

void getForwardVector(float forward[], int length,
                      int offset, HeadTransform* this_);

void getUpVector(float up[], int length,
                 int offset, HeadTransform* this_);

void getRightVector(float right[], int length,
                    int offset, HeadTransform* this_);

void getQuaternion(float quaternion[], int length,
                   int offset, HeadTransform* this_);

void getEulerAngles(float eulerAngles[], int length,
                    int offset, HeadTransform* this_);

void getTranslation(float translation[], int length, int offset,
                    HeadTransform* this_);
#endif /* HEADTRANSFORM_H */
