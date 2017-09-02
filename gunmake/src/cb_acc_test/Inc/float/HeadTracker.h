#ifndef HEADTRACKER_H
#define HEADTRACKER_H
#include "../base.h"
#include "OrientationEKF.h"
#include "GyroscopeBiasEstimator.h"
#include "SensorEvent.h"

typedef struct HeadTracker_ {
    float* ekfToHeadTracker;
    float Instance_ekfToHeadTracker[16];
    float* sensorToDisplay;
    float Instance_sensorToDisplay[16];
    float displayRotation; /* -3.4028235E38;-Float.MAX_VALUE; TODO*/
    float* neckModelTranslation;
    float Instance_neckModelTranslation[16];
    float* tmpHeadView;
    float Instance_tmpHeadView[16];
    float* tmpHeadView2;
    float Instance_tmpHeadView2[16];
    float neckModelFactor;
    bool tracking;
    OrientationEKF* tracker;
    OrientationEKF Instance_OrientationEKF;
    GyroscopeBiasEstimator* gyroBiasEstimator;
    GyroscopeBiasEstimator Instance_gyroBiasEstimator;
    long long latestGyroEventClockTimeNs;
    bool firstGyroValue;
    float* initialSystemGyroBias;
    float Instance_initialSystemGyroBias[3];
    Vector3d* gyroBias;
    Vector3d Instance_gyroBias;
    Vector3d* latestGyro;
    Vector3d Instance_latestGyro;
    Vector3d* latestAcc;
    Vector3d Instance_latestAcc;

    /*Object neckModelFactorMutex = new Object(); TODO*/
    /*Object gyroBiasEstimatorMutex = new Object(); TODO*/
    /*SensorEventProvider sensorEventProvider; TODO*/
    /*Clock clock; TODO*/
} HeadTracker;

void new_HeadTracker(HeadTracker* this_);
HeadTracker* get_HeadTracker_Instance();
void onSensorChanged(SensorEvent* event, HeadTracker* this_); /* TODO */
void onAccuracyChanged(/*Sensor* sensor, */int accuracy,
        HeadTracker* this_);  /* TODO */
void startTracking(HeadTracker* this_);
void resetTracker(HeadTracker* this_);
void stopTracking(HeadTracker* this_);
void setNeckModelEnabled(bool enabled, HeadTracker* this_);
float getNeckModelFactor(HeadTracker* this_);
void setNeckModelFactor(float factor, HeadTracker* this_);
void setGyroBiasEstimationEnabled(bool enabled, HeadTracker* this_);
bool getGyroBiasEstimationEnabled(HeadTracker* this_);
void getLastHeadView(float headView[], int length, int offset,
                     HeadTracker* this_);
/* Matrix3x3d* getCurrentPoseForTest(); */
void setGyroBiasEstimator(GyroscopeBiasEstimator* estimator,
                          HeadTracker* this_);
#endif  /* HEADTRACKER_H */
