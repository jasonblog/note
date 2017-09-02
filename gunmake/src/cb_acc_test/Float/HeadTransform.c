#include <stdio.h>
#include <math.h>
#include <string.h>
#include "../Inc/float/HeadTransform.h"
#include "../Inc/float/Matrix.h"

void new_HeadTransform(HeadTransform* this_)
{
    this_->headView = this_->Instance_headView;  /*TODO initial array ??*/
    setIdentityM(this_->headView, 0);
}

void arraycopy(float* src, int srcPos, float* dest,
               int destPos, int length)
{
    int i, j;

    for (i = 0; i < srcPos; i++) {
        src++;
    }

    for (j = 0; j < destPos; j++) {
        dest++;
    }

    memcpy(dest, src, length);
}

float* getHeadView_l(HeadTransform* this_)
{
    /*TODO*/
    return this_->headView;
}

void getHeadView(float headView[], int length,
                 int offset, HeadTransform* this_)
{
    /*TODO*/
    if (offset + 16 > length) {
        printf("Not enough space to write the result \n");
    } else {
        arraycopy(this_->headView, 0, headView, offset, 16);
    }
}

void getForwardVector(float forward[], int length,
                      int offset, HeadTransform* this_)
{
    int i;

    if (offset + 3 > length) {
        printf("Not enough space to write the result \n");
    } else {
        for (i = 0; i < 3; ++i) {
            forward[i + offset] = -this_->headView[8 + i];
        }

    }
}

void getUpVector(float up[], int length, int offset,
                 HeadTransform* this_)
{
    int i;

    if (offset + 3 > length) {
        printf("Not enough space to write the result \n");
    } else {
        for (i = 0; i < 3; ++i) {
            up[i + offset] = this_->headView[4 + i];
        }

    }
}

void getRightVector(float right[], int length,
                    int offset, HeadTransform* this_)
{
    int i;

    if (offset + 3 > length) {
        printf("Not enough space to write the result \n");
    } else {
        for (i = 0; i < 3; ++i) {
            right[i + offset] = this_->headView[i];
        }

    }
}

void getQuaternion(float quaternion[], int length,
                   int offset, HeadTransform* this_)
{
    /*TODO*/
    if (offset + 4 > length) {
        printf("Not enough space to write the result \n");
    } else {
        float* m = this_->headView;
        float t = m[0] + m[5] + m[10];
        float x;
        float y;
        float z;
        float w;
        float s;

        if (t >= 0.0F) {
            s = sqrt(t + 1.0F);
            w = 0.5F * s;
            s = 0.5F / s;
            x = (m[9] - m[6]) * s;
            y = (m[2] - m[8]) * s;
            z = (m[4] - m[1]) * s;
        } else if (m[0] > m[5] && m[0] > m[10]) {
            s = sqrt(1.0F + m[0] - m[5] - m[10]);
            x = s * 0.5F;
            s = 0.5F / s;
            y = (m[4] + m[1]) * s;
            z = (m[2] + m[8]) * s;
            w = (m[9] - m[6]) * s;
        } else if (m[5] > m[10]) {
            s = sqrt(1.0F + m[5] - m[0] - m[10]);
            y = s * 0.5F;
            s = 0.5F / s;
            x = (m[4] + m[1]) * s;
            z = (m[9] + m[6]) * s;
            w = (m[2] - m[8]) * s;
        } else {
            s = sqrt(1.0F + m[10] - m[0] - m[5]);
            z = s * 0.5F;
            s = 0.5F / s;
            x = (m[2] + m[8]) * s;
            y = (m[9] + m[6]) * s;
            w = (m[4] - m[1]) * s;
        }

        quaternion[offset + 0] = x;
        quaternion[offset + 1] = y;
        quaternion[offset + 2] = z;
        quaternion[offset + 3] = w;
    }
}

void getEulerAngles(float eulerAngles[], int length,
                    int offset, HeadTransform* this_)
{
    /*TODO*/
    if (offset + 3 > length) {
        printf("Not enough space to write the result \n");
    } else {
        float pitch = (float)asin((float)this_->headView[6]);
        float yaw;
        float roll;

        if (sqrt(1.0F - this_->headView[6] * this_->headView[6]) >=
            0.01F) {
            yaw = (float)atan2((float)(-this_->headView[2]),
                               (float)this_->headView[10]);

            roll = (float)atan2((float)(-this_->headView[4]),
                                (float)this_->headView[5]);
        } else {
            yaw = 0.0F;
            roll = (float)atan2((float)this_->headView[1],
                                (float)this_->headView[0]);
        }

        eulerAngles[offset + 0] = -pitch;
        eulerAngles[offset + 1] = -yaw;
        eulerAngles[offset + 2] = -roll;
    }
}

void getTranslation(float translation[], int length,
                    int offset, HeadTransform* this_)
{
    int i;

    if (offset + 3 > length) {
        printf("Not enough space to write the result \n");
    } else {
        for (i = 0; i < 3; ++i) {
            translation[i + offset] = this_->headView[12 + i];
        }

    }
}
