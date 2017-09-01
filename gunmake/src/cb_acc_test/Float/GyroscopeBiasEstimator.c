#include <stdio.h>
#include "../Inc/float/GyroscopeBiasEstimator.h"

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

static void updateGyroBias(Vector3d* gyro, long long sensorTimestampNs,
                           GyroscopeBiasEstimator* this_);

static void updateGyroBias(Vector3d* gyro, long long sensorTimestampNs,
                           GyroscopeBiasEstimator* this_)
{
    /*TODO*/
    if (Vector3d_length(gyro) < 0.3499999940395355) {
        float updateWeight = max(0.0,
                                 1.0 - Vector3d_length(gyro) / 0.3499999940395355);
        updateWeight *= updateWeight;

        addWeightedSample(getFilteredData(this_->gyroLowPass),
                          sensorTimestampNs, updateWeight,
                          this_->gyroBiasLowPass);
    }
}

void new_GyroscopeBiasEstimator(GyroscopeBiasEstimator* this_)
{
    GyroscopeBiasEstimator_reset(this_);
}

void GyroscopeBiasEstimator_reset(GyroscopeBiasEstimator* this_)
{
    this_->smoothedGyroDiff = &this_->Instance_smoothedGyroDiff;
    new_Vector3d(this_->smoothedGyroDiff);

    this_->smoothedAccelDiff = &this_->Instance_smoothedAccelDiff;
    new_Vector3d(this_->smoothedAccelDiff);

    this_->accelLowPass = &this_->Instance_accelLowPass;
    new_LowPassFilter(1.0, this_->accelLowPass);

    this_->gyroLowPass = &this_->Instance_gyroLowPass;
    new_LowPassFilter(10.0, this_->gyroLowPass);

    this_->gyroBiasLowPass = &this_->Instance_gyroBiasLowPass;
    new_LowPassFilter(0.15000000596046448, this_->gyroBiasLowPass);

    this_->isAccelStatic = &this_->Instance_isAccelStatic;
    new_IsStaticCounter(10, this_->isAccelStatic);

    this_->isGyroStatic = &this_->Instance_isGyroStatic;
    new_IsStaticCounter(10, this_->isGyroStatic);
}

void new_IsStaticCounter(int minStaticFrames, IsStaticCounter* this_)
{
    this_->minStaticFrames = minStaticFrames;
    this_->consecutiveIsStatic = 0;
}

void processGyroscope(Vector3d* gyro, long long sensorTimestampNs,
                      GyroscopeBiasEstimator* this_)
{
    /*TODO*/
    addSample(gyro, sensorTimestampNs, this_->gyroLowPass);
    Vector3d_sub(gyro, getFilteredData(this_->gyroLowPass),
                 this_->smoothedGyroDiff);
    appendFrame(Vector3d_length(this_->smoothedGyroDiff) <
                0.00800000037997961, this_->isGyroStatic);

    if (isRecentlyStatic(this_->isGyroStatic) &&
        isRecentlyStatic(this_->isAccelStatic)) {
        updateGyroBias(gyro, sensorTimestampNs, this_);
    }
}

void processAccelerometer(Vector3d* accel,
                          long long sensorTimestampNs, GyroscopeBiasEstimator* this_)
{
    /*TODO*/
    addSample(accel, sensorTimestampNs, this_->accelLowPass);
    Vector3d_sub(accel, getFilteredData(this_->accelLowPass),
                 this_->smoothedAccelDiff);

#ifdef DEBUG
    printf("processAccelerometer smoothedAccelDiff x=%f smoothedAccelDiff y=%f smoothedAccelDiff z=%f\n",
           this_->smoothedAccelDiff->x, this_->smoothedAccelDiff->y,
           this_->smoothedAccelDiff->z);
#endif

    appendFrame(Vector3d_length(this_->smoothedAccelDiff) < 0.5,
                this_->isAccelStatic);
}

void getGyroBias(Vector3d* result, GyroscopeBiasEstimator* this_)
{
    /*TODO*/
    if (getNumSamples(this_->gyroBiasLowPass) < 30) {
        Vector3d_setZero(result);
    } else {
        Vector3d_set_o(getFilteredData(this_->gyroBiasLowPass), result);
        float rampUpRatio = min(1.0,
                                (float)(getNumSamples(this_->gyroBiasLowPass) - 30) / 100.0);
        Vector3d_scale(rampUpRatio, result);
    }

#ifdef DEBUG
    printf("getGyroBias result x=%f, y=%f, z=%f\n", result->x, result->y,
           result->z);
#endif
}

void appendFrame(bool isStatic, IsStaticCounter* this_)
{
#ifdef DEBUG
    printf("appendFrame isStatic=%d\n", isStatic);
#endif

    /*TODO*/
    if (!isStatic) {
        this_->consecutiveIsStatic = 0;
    } else {
        ++this_->consecutiveIsStatic;
    }
}

bool isRecentlyStatic(IsStaticCounter* this_)
{
    /*TODO*/
    return this_->consecutiveIsStatic >= this_->minStaticFrames;
}
