#ifndef GYROSCOPEBIASESTIMATOR_H
#define GYROSCOPEBIASESTIMATOR_H
#include "../base.h"
#include "LowPassFilter.h"
#include "Vector3d.h"

typedef struct IsStaticCounter_ {
    int minStaticFrames;
    int consecutiveIsStatic;
} IsStaticCounter;

typedef struct GyroscopeBiasEstimator_ {
    LowPassFilter* accelLowPass;
    LowPassFilter Instance_accelLowPass;
    LowPassFilter* gyroLowPass;
    LowPassFilter Instance_gyroLowPass;
    LowPassFilter* gyroBiasLowPass;
    LowPassFilter Instance_gyroBiasLowPass;
    Vector3d* smoothedGyroDiff;
    Vector3d Instance_smoothedGyroDiff;
    Vector3d* smoothedAccelDiff;
    Vector3d Instance_smoothedAccelDiff;
    IsStaticCounter* isAccelStatic;
    IsStaticCounter Instance_isAccelStatic;
    IsStaticCounter* isGyroStatic;
    IsStaticCounter Instance_isGyroStatic;
} GyroscopeBiasEstimator;


void new_GyroscopeBiasEstimator(GyroscopeBiasEstimator* this_);
void new_IsStaticCounter(int minStaticFrames, IsStaticCounter* this_);
void GyroscopeBiasEstimator_reset(GyroscopeBiasEstimator* this_);
void processGyroscope(Vector3d* gyro, long long sensorTimestampNs,
                      GyroscopeBiasEstimator* this_);
void processAccelerometer(Vector3d* accel, long long sensorTimestampNs,
                          GyroscopeBiasEstimator* this_);
void getGyroBias(Vector3d* result, GyroscopeBiasEstimator* this_);
void appendFrame(bool isStatic, IsStaticCounter* this_);
bool isRecentlyStatic(IsStaticCounter* this_);
#endif /* GYROSCOPEBIASESTIMATOR_H */
