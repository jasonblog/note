#ifndef ORIENTATIONEKF_H
#define ORIENTATIONEKF_H
#include "../base.h"
#include "Vector3d.h"
#include "Matrix3x3d.h"

static const float ns2s = 1.0e-9f;
static const float min_accel_noise_sigma = 0.75;
static const float max_accel_noise_sigma = 7.0;

typedef struct OrientationEKF_ {
    float* rotationMatrix;
    float Instance_rotationMatrix[16];
    Matrix3x3d* so3SensorFromWorld;
    Matrix3x3d Instance_so3SensorFromWorld;
    Matrix3x3d* so3LastMotion;
    Matrix3x3d Instance_so3LastMotion;
    Matrix3x3d* mP;
    Matrix3x3d Instance_mP;
    Matrix3x3d* mQ;
    Matrix3x3d Instance_mQ;
    Matrix3x3d* mR;
    Matrix3x3d Instance_mR;
    Matrix3x3d* mRaccel;
    Matrix3x3d Instance_mRaccel;
    Matrix3x3d* mS;
    Matrix3x3d Instance_mS;
    Matrix3x3d* mH;
    Matrix3x3d Instance_mH;
    Matrix3x3d* mK;
    Matrix3x3d Instance_mK;
    Vector3d* mNu;
    Vector3d Instance_mNu;
    Vector3d* mz;
    Vector3d Instance_mz;
    Vector3d* mh;
    Vector3d Instance_mh;
    Vector3d* mu;
    Vector3d Instance_mu;
    Vector3d* mx;
    Vector3d Instance_mx;
    Vector3d* down;
    Vector3d Instance_down;
    Vector3d* north;
    Vector3d Instance_north;
    long long sensorTimeStampGyro;
    Vector3d* lastGyro;
    Vector3d Instance_lastGyro;
    float previousAccelNorm;
    float movingAverageAccelNormChange;
    float filteredGyroTimestep;
    bool timestepFilterInit;
    int numGyroTimestepSamples;
    bool gyroFilterValid;
    Matrix3x3d* getPredictedGLMatrixTempM1;
    Matrix3x3d Instance_getPredictedGLMatrixTempM1;
    Matrix3x3d* getPredictedGLMatrixTempM2;
    Matrix3x3d Instance_getPredictedGLMatrixTempM2;
    Vector3d* getPredictedGLMatrixTempV1;
    Vector3d Instance_getPredictedGLMatrixTempV1;
    Matrix3x3d* setHeadingDegreesTempM1;
    Matrix3x3d Instance_setHeadingDegreesTempM1;
    Matrix3x3d* processGyroTempM1;
    Matrix3x3d Instance_processGyroTempM1;
    Matrix3x3d* processGyroTempM2;
    Matrix3x3d Instance_processGyroTempM2;
    Matrix3x3d* processAccTempM1;
    Matrix3x3d Instance_processAccTempM1;
    Matrix3x3d* processAccTempM2;
    Matrix3x3d Instance_processAccTempM2;
    Matrix3x3d* processAccTempM3;
    Matrix3x3d Instance_processAccTempM3;
    Matrix3x3d* processAccTempM4;
    Matrix3x3d Instance_processAccTempM4;
    Matrix3x3d* processAccTempM5;
    Matrix3x3d Instance_processAccTempM5;
    Vector3d* processAccTempV1;
    Vector3d Instance_processAccTempV1;
    Vector3d* processAccTempV2;
    Vector3d Instance_processAccTempV2;
    Vector3d* processAccVDelta;
    Vector3d Instance_processAccVDelta;
    Vector3d* processMagTempV1;
    Vector3d Instance_processMagTempV1;
    Vector3d* processMagTempV2;
    Vector3d Instance_processMagTempV2;
    Vector3d* processMagTempV3;
    Vector3d Instance_processMagTempV3;
    Vector3d* processMagTempV4;
    Vector3d Instance_processMagTempV4;
    Vector3d* processMagTempV5;
    Vector3d Instance_processMagTempV5;
    Matrix3x3d* processMagTempM1;
    Matrix3x3d Instance_processMagTempM1;
    Matrix3x3d* processMagTempM2;
    Matrix3x3d Instance_processMagTempM2;
    Matrix3x3d* processMagTempM4;
    Matrix3x3d Instance_processMagTempM4;
    Matrix3x3d* processMagTempM5;
    Matrix3x3d Instance_processMagTempM5;
    Matrix3x3d* processMagTempM6;
    Matrix3x3d Instance_processMagTempM6;
    Matrix3x3d* updateCovariancesAfterMotionTempM1;
    Matrix3x3d Instance_updateCovariancesAfterMotionTempM1;
    Matrix3x3d* updateCovariancesAfterMotionTempM2;
    Matrix3x3d Instance_updateCovariancesAfterMotionTempM2;
    Matrix3x3d* accObservationFunctionForNumericalJacobianTempM;
    Matrix3x3d Instance_accObservationFunctionForNumericalJacobianTempM;
    Matrix3x3d* magObservationFunctionForNumericalJacobianTempM;
    Matrix3x3d Instance_magObservationFunctionForNumericalJacobianTempM;
    bool alignedToGravity;
    bool alignedToNorth;
} OrientationEKF;

void new_OrientationEKF(OrientationEKF* this_);
void OrientationEKF_reset(OrientationEKF* this_);
bool isReady(OrientationEKF* this_);
float getHeadingDegrees(OrientationEKF* this_);
void setHeadingDegrees(float heading, OrientationEKF* this_);
float* getGLMatrix(OrientationEKF* this_);

float* getPredictedGLMatrix(float secondsafterlastgyroevent,
                            OrientationEKF* this_);

Matrix3x3d* getRotationMatrix(OrientationEKF* this_);
void arrayAssign(float (*data)[3], Matrix3x3d* m);
bool isAlignedToGravity(OrientationEKF* this_);
bool isAlignedToNorth(OrientationEKF* this_);

void processGyro(Vector3d* gyro,
                 long long sensortimestamp,
                 OrientationEKF* this_);
void processAcc(Vector3d* acc,
                long long sensortimestamp,
                OrientationEKF* this_);
void processMag(float mag[],
                long long sensortimestamp,
                OrientationEKF* this_);
#endif /* ORIENTATIONEKF_H */
