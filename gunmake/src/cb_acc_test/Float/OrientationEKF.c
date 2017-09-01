#include <stdio.h>
#include <math.h>
#include "../Inc/float/So3Util.h"
#include "../Inc/float/OrientationEKF.h"

static void updateAccelCovariance(float currentAccelNorm,
                                  OrientationEKF* this_);
static float* glMatrixFromSo3(Matrix3x3d* so3, OrientationEKF* this_);
static void filterGyroTimestep(float timeStep, OrientationEKF* this_);
static void updateCovariancesAfterMotion(OrientationEKF* this_);

static void accObservationFunctionForNumericalJacobian(Matrix3x3d*
        so3SensorFromWorldPred, Vector3d* result, OrientationEKF* this_);

static void magObservationFunctionForNumericalJacobian(Matrix3x3d*
        so3SensorFromWorldPred , Vector3d* result, OrientationEKF* this_);

void new_OrientationEKF(OrientationEKF* this_)
{
    OrientationEKF_reset(this_);
}

void OrientationEKF_reset(OrientationEKF* this_)
{
    Init_So3Util();
    this_->rotationMatrix = this_->Instance_rotationMatrix;

    this_->so3SensorFromWorld = &this_->Instance_so3SensorFromWorld;
    new_Matrix3x3d(this_->so3SensorFromWorld);

    this_->so3LastMotion = &this_->Instance_so3LastMotion;
    new_Matrix3x3d(this_->so3LastMotion);

    this_->mP = &this_->Instance_mP;
    new_Matrix3x3d(this_->mP);

    this_->mQ = &this_->Instance_mQ;
    new_Matrix3x3d(this_->mQ);

    this_->mR = &this_->Instance_mR;
    new_Matrix3x3d(this_->mR);

    this_->mRaccel = &this_->Instance_mRaccel;
    new_Matrix3x3d(this_->mRaccel);

    this_->mS = &this_->Instance_mS;
    new_Matrix3x3d(this_->mS);

    this_->mH = &this_->Instance_mH;
    new_Matrix3x3d(this_->mH);

    this_->mK = &this_->Instance_mK;
    new_Matrix3x3d(this_->mK);

    this_->mNu = &this_->Instance_mNu;
    new_Vector3d(this_->mNu);

    this_->mz = &this_->Instance_mz;
    new_Vector3d(this_->mz);

    this_->mh = &this_->Instance_mh;
    new_Vector3d(this_->mh);

    this_->mu = &this_->Instance_mu;
    new_Vector3d(this_->mu);

    this_->mx = &this_->Instance_mx;
    new_Vector3d(this_->mx);

    this_->down = &this_->Instance_down;
    new_Vector3d(this_->down);

    this_->north = &this_->Instance_north;
    new_Vector3d(this_->north);

    this_->lastGyro = &this_->Instance_lastGyro;
    new_Vector3d(this_->lastGyro);

    this_->getPredictedGLMatrixTempM1 = &this_->Instance_getPredictedGLMatrixTempM1;
    new_Matrix3x3d(this_->getPredictedGLMatrixTempM1);

    this_->getPredictedGLMatrixTempM2 = &this_->Instance_getPredictedGLMatrixTempM2;
    new_Matrix3x3d(this_->getPredictedGLMatrixTempM2);

    this_->getPredictedGLMatrixTempV1 = &this_->Instance_getPredictedGLMatrixTempV1;
    new_Vector3d(this_->getPredictedGLMatrixTempV1);

    this_->setHeadingDegreesTempM1 = &this_->Instance_setHeadingDegreesTempM1;
    new_Matrix3x3d(this_->setHeadingDegreesTempM1);

    this_->processGyroTempM1 = &this_->Instance_processGyroTempM1;
    new_Matrix3x3d(this_->processGyroTempM1);

    this_->processGyroTempM2 = &this_->Instance_processGyroTempM2;
    new_Matrix3x3d(this_->processGyroTempM2);

    this_->processAccTempM1 = &this_->Instance_processAccTempM1;
    new_Matrix3x3d(this_->processAccTempM1);

    this_->processAccTempM2 = &this_->Instance_processAccTempM2;
    new_Matrix3x3d(this_->processAccTempM2);

    this_->processAccTempM3 = &this_->Instance_processAccTempM3;
    new_Matrix3x3d(this_->processAccTempM3);

    this_->processAccTempM4 = &this_->Instance_processAccTempM4;
    new_Matrix3x3d(this_->processAccTempM4);

    this_->processAccTempM5 = &this_->Instance_processAccTempM5;
    new_Matrix3x3d(this_->processAccTempM5);

    this_->processAccTempV1 = &this_->Instance_processAccTempV1;
    new_Vector3d(this_->processAccTempV1);

    this_->processAccTempV2 = &this_->Instance_processAccTempV2;
    new_Vector3d(this_->processAccTempV2);

    this_->processAccVDelta = &this_->Instance_processAccVDelta;
    new_Vector3d(this_->processAccVDelta);

    this_->processMagTempV1 = &this_->Instance_processMagTempV1;
    new_Vector3d(this_->processMagTempV1);

    this_->processMagTempV2 = &this_->Instance_processMagTempV2;
    new_Vector3d(this_->processMagTempV2);

    this_->processMagTempV3 = &this_->Instance_processMagTempV3;
    new_Vector3d(this_->processMagTempV3);

    this_->processMagTempV4 = &this_->Instance_processMagTempV4;
    new_Vector3d(this_->processMagTempV4);

    this_->processMagTempV5 = &this_->Instance_processMagTempV5;
    new_Vector3d(this_->processMagTempV5);

    this_->processMagTempM1 = &this_->Instance_processMagTempM1;
    new_Matrix3x3d(this_->processMagTempM1);

    this_->processMagTempM2 = &this_->Instance_processMagTempM2;
    new_Matrix3x3d(this_->processMagTempM2);

    this_->processMagTempM4 = &this_->Instance_processMagTempM4;
    new_Matrix3x3d(this_->processMagTempM4);

    this_->processMagTempM5 = &this_->Instance_processMagTempM5;
    new_Matrix3x3d(this_->processMagTempM5);

    this_->processMagTempM6 = &this_->Instance_processMagTempM6;
    new_Matrix3x3d(this_->processMagTempM6);

    this_->updateCovariancesAfterMotionTempM1 =
        &this_->Instance_updateCovariancesAfterMotionTempM1;
    new_Matrix3x3d(this_->updateCovariancesAfterMotionTempM1);

    this_->updateCovariancesAfterMotionTempM2 =
        &this_->Instance_updateCovariancesAfterMotionTempM2;
    new_Matrix3x3d(this_->updateCovariancesAfterMotionTempM2);

    this_->accObservationFunctionForNumericalJacobianTempM =
        &this_->Instance_accObservationFunctionForNumericalJacobianTempM;
    new_Matrix3x3d(this_->accObservationFunctionForNumericalJacobianTempM);

    this_->magObservationFunctionForNumericalJacobianTempM =
        &this_->Instance_magObservationFunctionForNumericalJacobianTempM;
    new_Matrix3x3d(this_->magObservationFunctionForNumericalJacobianTempM);

    this_->sensorTimeStampGyro = 0L;
    Matrix3x3d_setIdentity(this_->so3SensorFromWorld);
    Matrix3x3d_setIdentity(this_->so3LastMotion);
    Matrix3x3d_setZero(this_->mP);
    Matrix3x3d_setSameDiagonal(25.0, this_->mP);
    Matrix3x3d_setZero(this_->mQ);
    Matrix3x3d_setSameDiagonal(1.0, this_->mQ);
    Matrix3x3d_setZero(this_->mR);
    Matrix3x3d_setSameDiagonal(0.0625, this_->mR);
    Matrix3x3d_setZero(this_->mRaccel);
    Matrix3x3d_setSameDiagonal(0.5625, this_->mRaccel);
    Matrix3x3d_setZero(this_->mS);
    Matrix3x3d_setZero(this_->mH);
    Matrix3x3d_setZero(this_->mK);
    Vector3d_setZero(this_->mNu);
    Vector3d_setZero(this_->mz);
    Vector3d_setZero(this_->mh);
    Vector3d_setZero(this_->mu);
    Vector3d_setZero(this_->mx);
    Vector3d_set(0.0, 0.0, 9.81, this_->down);
    Vector3d_set(0.0, 1.0, 0.0, this_->north);
    this_->alignedToGravity = false;
    this_->alignedToNorth = false;
    this_->previousAccelNorm = 0.0;
    this_->movingAverageAccelNormChange = 0.0;
    this_->filteredGyroTimestep = 0.0f;
    this_->timestepFilterInit = false;
    this_->numGyroTimestepSamples = 0;
    this_->gyroFilterValid = true;
}

bool isReady(OrientationEKF* this_)
{
    /*TODO*/
    return this_->alignedToGravity;
}

float getHeadingDegrees(OrientationEKF* this_)
{
    float x = Matrix3x3d_get(2, 0, this_->so3SensorFromWorld);
    float y = Matrix3x3d_get(2, 1, this_->so3SensorFromWorld);
    float mag = sqrt(x * x + y * y);

    if (mag < 0.1) {
        return 0.0;
    } else {
        float heading = -90.0 - atan2(y, x) / 3.141592653589793 * 180.0;

        if (heading < 0.0) {
            heading += 360.0;
        }

        if (heading >= 360.0) {
            heading -= 360.0;
        }

        return heading;
    }
}

void setHeadingDegrees(float heading, OrientationEKF* this_)
{
    float currentHeading = getHeadingDegrees(this_);
    float deltaHeading = heading - currentHeading;
    float s = sin(deltaHeading / 180.0 * 3.141592653589793);
    float c = cos(deltaHeading / 180.0 * 3.141592653589793);
    float deltaHeadingRotationVals[3][3] = {{c, -s, 0.0}, {s, c, 0.0}, {0.0, 0.0, 1.0}};
    arrayAssign(deltaHeadingRotationVals, this_->setHeadingDegreesTempM1);
    Matrix3x3d_mult_M(this_->so3SensorFromWorld, this_->setHeadingDegreesTempM1,
                      this_->so3SensorFromWorld);
}

float* getGLMatrix(OrientationEKF* this_)
{
    return glMatrixFromSo3(this_->so3SensorFromWorld, this_);
}

float* getPredictedGLMatrix(float secondsAfterLastGyroEvent,
                            OrientationEKF* this_)
{
    /*TODO*/

    Vector3d* pmu = this_->getPredictedGLMatrixTempV1;
    Vector3d_set_o(this_->lastGyro, pmu);
    Vector3d_scale(-secondsAfterLastGyroEvent, pmu);
    Matrix3x3d* so3PredictedMotion = this_->getPredictedGLMatrixTempM1;
    So3Util_sO3FromMu(pmu, so3PredictedMotion);
    Matrix3x3d* so3PredictedState = this_->getPredictedGLMatrixTempM2;
    Matrix3x3d_mult_M(so3PredictedMotion, this_->so3SensorFromWorld,
                      so3PredictedState);

    return glMatrixFromSo3(so3PredictedState, this_);
}

Matrix3x3d* getRotationMatrix(OrientationEKF* this_)
{
    return this_->so3SensorFromWorld;
}

void arrayAssign(float (*data)[3], Matrix3x3d* m)
{

}

bool isAlignedToGravity(OrientationEKF* this_)
{
    return this_->alignedToGravity;
}

bool isAlignedToNorth(OrientationEKF* this_)
{
    return this_->alignedToNorth;
}

void processGyro(Vector3d* gyro, long long sensorTimeStamp,
                 OrientationEKF* this_)
{
#ifdef DEBUG
    printf("processGyro init gyro x=%f, y=%f, z=%f, sensorTimeStamp=%lld\n",
           gyro->x, gyro->y, gyro->z, sensorTimeStamp);
#endif

    /*TODO*/
    // float kTimeThreshold = 0.04F;
    // float kdTdefault = 0.01F;

#ifdef DEBUG
    printf("processGyro sensorTimeStampGyro=%lld\n", this_->sensorTimeStampGyro);
#endif

    if (this_->sensorTimeStampGyro != 0L) {
        float dT = (float)(sensorTimeStamp - this_->sensorTimeStampGyro) * 1.0E-9F;

        if (dT > 0.04F) {
            dT = this_->gyroFilterValid ? this_->filteredGyroTimestep : 0.01F;
        } else {
            filterGyroTimestep(dT, this_);
        }

#ifdef DEBUG
        printf("dT = %f, sensorTimeStamp - sensorTimeStampGyro = %lld\n", dT,
               sensorTimeStamp - this_->sensorTimeStampGyro);
#endif

        Vector3d_set_o(gyro, this_->mu);
        Vector3d_scale((float)(-dT), this_->mu);
        So3Util_sO3FromMu(this_->mu, this_->so3LastMotion);
        Matrix3x3d_set_o(this_->so3SensorFromWorld, this_->processGyroTempM1);
        Matrix3x3d_mult_M(this_->so3LastMotion, this_->so3SensorFromWorld,
                          this_->processGyroTempM1);

#ifdef DEBUG

        for (int i = 0; i < 9; i++) {
            printf("processGyro mult processGyroTempM1.m[%d] = %f\n", i,
                   this_->processGyroTempM1->m[i]);
        }

#endif

        Matrix3x3d_set_o(this_->processGyroTempM1, this_->so3SensorFromWorld);
        updateCovariancesAfterMotion(this_);
        Matrix3x3d_set_o(this_->mQ, this_->processGyroTempM2);
        Matrix3x3d_scale((float)(dT * dT), this_->processGyroTempM2);
        Matrix3x3d_plusEquals(this_->processGyroTempM2, this_->mP);

#ifdef DEBUG

        for (int i = 0; i < 9; i++) {
            printf("processGyro plusEquals mP.m[%d] = %f\n", i, this_->mP->m[i]);
        }

#endif
    }

    this_->sensorTimeStampGyro = sensorTimeStamp;
    Vector3d_set_o(gyro, this_->lastGyro);

#ifdef DEBUG
    printf("processGyro lastGyro set lastGyro x=%f, y=%f, z=%f\n",
           this_->lastGyro->x,
           this_->lastGyro->y,
           this_->lastGyro->z);
#endif

}

void updateAccelCovariance(float currentAccelNorm, OrientationEKF* this_)
{
    /*TODO*/
    float currentAccelNormChange = fabs((currentAccelNorm -
                                         this_->previousAccelNorm));
    this_->previousAccelNorm = currentAccelNorm;
    // float kSmoothingFactor = 0.5D;
    this_->movingAverageAccelNormChange = 0.5 * currentAccelNormChange + 0.5 *
                                          this_->movingAverageAccelNormChange;
    // float kMaxAccelNormChange = 0.15D;
    float normChangeRatio = this_->movingAverageAccelNormChange / 0.15;
    float accelNoiseSigma = fmin(7.0, 0.75 + normChangeRatio * 6.25);

#ifdef DEBUG
    printf("accelNoiseSigma * accelNoiseSigma = %f\n",
           accelNoiseSigma * accelNoiseSigma);
#endif

    Matrix3x3d_setSameDiagonal(accelNoiseSigma * accelNoiseSigma, this_->mRaccel);
}

void processAcc(Vector3d* acc, long long sensorTimeStamp, OrientationEKF* this_)
{

#ifdef DEBUG
    printf("processAcc init acc x=%f, acc y=%f, acc z=%f, sensorTimeStamp=%lld\n",
           acc->x, acc->y, acc->z, sensorTimeStamp);
#endif

    /*TODO*/
    int dof;
    Vector3d_set_o(acc, this_->mz);
    updateAccelCovariance(Vector3d_length(this_->mz), this_);

    if (this_->alignedToGravity) {
        accObservationFunctionForNumericalJacobian(this_->so3SensorFromWorld,
                this_->mNu, this_);
        float eps = 1.0E-7;

        for (dof = 0; dof < 3; ++dof) {
            Vector3d* delta = this_->processAccVDelta;
            Vector3d_setZero(delta);
            Vector3d_setComponent(dof, eps, delta);
            So3Util_sO3FromMu(delta, this_->processAccTempM1);
            Matrix3x3d_mult_M(this_->processAccTempM1, this_->so3SensorFromWorld,
                              this_->processAccTempM2);
            accObservationFunctionForNumericalJacobian(this_->processAccTempM2,
                    this_->processAccTempV1, this_);
            Vector3d* withDelta = this_->processAccTempV1;
            Vector3d_sub(this_->mNu, withDelta, this_->processAccTempV2);
            Vector3d_scale(1.0 / eps, this_->processAccTempV2);
            Matrix3x3d_setColumn(dof, this_->processAccTempV2, this_->mH);
        }

        Matrix3x3d_transpose_r(this_->processAccTempM3, this_->mH);
        Matrix3x3d_mult_M(this_->mP, this_->processAccTempM3,
                          this_->processAccTempM4);
        Matrix3x3d_mult_M(this_->mH, this_->processAccTempM4,
                          this_->processAccTempM5);
        Matrix3x3d_add(this_->processAccTempM5, this_->mRaccel, this_->mS);
        Matrix3x3d_invert(this_->processAccTempM3, this_->mS);
        Matrix3x3d_transpose_r(this_->processAccTempM4, this_->mH);
        Matrix3x3d_mult_M(this_->processAccTempM4, this_->processAccTempM3,
                          this_->processAccTempM5);
        Matrix3x3d_mult_M(this_->mP, this_->processAccTempM5, this_->mK);
        Matrix3x3d_mult_V(this_->mK, this_->mNu, this_->mx);
        Matrix3x3d_mult_M(this_->mK, this_->mH, this_->processAccTempM3);
        Matrix3x3d_setIdentity(this_->processAccTempM4);
        Matrix3x3d_minusEquals(this_->processAccTempM3, this_->processAccTempM4);
        Matrix3x3d_mult_M(this_->processAccTempM4, this_->mP,
                          this_->processAccTempM3);
        Matrix3x3d_set_o(this_->processAccTempM3, this_->mP);
        So3Util_sO3FromMu(this_->mx, this_->so3LastMotion);
        Matrix3x3d_mult_M(this_->so3LastMotion, this_->so3SensorFromWorld,
                          this_->so3SensorFromWorld);

#ifdef DEBUG

        for (int i = 0; i < 9; ++i) {
            printf("processAcc so3SensorFromWorld mult [%d]=%f\n", i,
                   this_->so3SensorFromWorld->m[i]);
        }

#endif

        updateCovariancesAfterMotion(this_);
    } else {

#ifdef DEBUG

        for (int i = 0; i < 9; ++i) {
            printf("processAcc So3Util_sO3FromTwoVec so3SensorFromWorld [1] [%d]=%f\n", i,
                   this_->so3SensorFromWorld->m[i]);
        }

#endif

        So3Util_sO3FromTwoVec(this_->down, this_->mz, this_->so3SensorFromWorld);

#ifdef DEBUG

        for (int i = 0; i < 9; ++i) {
            printf("processAcc So3Util_sO3FromTwoVec so3SensorFromWorld [2] [%d]=%f\n", i,
                   this_->so3SensorFromWorld->m[i]);
        }

#endif

        this_->alignedToGravity = true;
    }
}

void processMag(float mag[], long long sensorTimeStamp, OrientationEKF* this_)
{

#ifdef DEBUG
    printf("processMag init Mag_x=%f, Mag_y=%f, Mag_z=%f, sensorTimeStamp=%lld\n",
           mag[0], mag[1], mag[2], sensorTimeStamp);
#endif

    /*TODO*/
    int dof;

    if (this_->alignedToGravity) {
        Vector3d_set((float)mag[0], (float)mag[1], (float)mag[2], this_->mz);
        Vector3d_normalize(this_->mz);
        Vector3d downInSensorFrame;/* = new Vector3d();*/
        Matrix3x3d_getColumn(2, &downInSensorFrame, this_->so3SensorFromWorld);
        Vector3d_cross(this_->mz, &downInSensorFrame, this_->processMagTempV1);
        Vector3d* perpToDownAndMag = this_->processMagTempV1;
        Vector3d_normalize(perpToDownAndMag);
        Vector3d_cross(&downInSensorFrame, perpToDownAndMag, this_->processMagTempV2);
        Vector3d* magHorizontal = this_->processMagTempV2;
        Vector3d_normalize(magHorizontal);
        Vector3d_set_o(magHorizontal, this_->mz);

        if (this_->alignedToNorth) {
            magObservationFunctionForNumericalJacobian(this_->so3SensorFromWorld,
                    this_->mNu, this_);
            float eps = 1.0E-7;

            for (dof = 0; dof < 3; ++dof) {
                Vector3d* delta = this_->processMagTempV3;
                Vector3d_setZero(delta);
                Vector3d_setComponent(dof, eps, delta);
                So3Util_sO3FromMu(delta, this_->processMagTempM1);
                Matrix3x3d_mult_M(this_->processMagTempM1, this_->so3SensorFromWorld,
                                  this_->processMagTempM2);
                magObservationFunctionForNumericalJacobian(this_->processMagTempM2,
                        this_->processMagTempV4, this_);
                Vector3d* withDelta = this_->processMagTempV4;
                Vector3d_sub(this_->mNu, withDelta, this_->processMagTempV5);
                Vector3d_scale(1.0 / eps, this_->processMagTempV5);
                Matrix3x3d_setColumn(dof, this_->processMagTempV5, this_->mH);
            }

            Matrix3x3d_transpose_r(this_->processMagTempM4, this_->mH);
            Matrix3x3d_mult_M(this_->mP, this_->processMagTempM4,
                              this_->processMagTempM5);
            Matrix3x3d_mult_M(this_->mH, this_->processMagTempM5,
                              this_->processMagTempM6);
            Matrix3x3d_add(this_->processMagTempM6, this_->mR, this_->mS);
            Matrix3x3d_invert(this_->processMagTempM4, this_->mS);
            Matrix3x3d_transpose_r(this_->processMagTempM5, this_->mH);
            Matrix3x3d_mult_M(this_->processMagTempM5, this_->processMagTempM4,
                              this_->processMagTempM6);
            Matrix3x3d_mult_M(this_->mP, this_->processMagTempM6, this_->mK);
            Matrix3x3d_mult_V(this_->mK, this_->mNu, this_->mx);
            Matrix3x3d_mult_M(this_->mK, this_->mH, this_->processMagTempM4);
            Matrix3x3d_setIdentity(this_->processMagTempM5);
            Matrix3x3d_minusEquals(this_->processMagTempM4, this_->processMagTempM5);
            Matrix3x3d_mult_M(this_->processMagTempM5, this_->mP,
                              this_->processMagTempM4);
            Matrix3x3d_set_o(this_->processMagTempM4, this_->mP);
            So3Util_sO3FromMu(this_->mx, this_->so3LastMotion);
            Matrix3x3d_mult_M(this_->so3LastMotion, this_->so3SensorFromWorld,
                              this_->processMagTempM4);
            Matrix3x3d_set_o(this_->processMagTempM4, this_->so3SensorFromWorld);


#ifdef DEBUG

            for (int i = 0; i < 9; i++) {
                printf("processMag so3SensorFromWorld[1] this.so3SensorFromWorld.m[%d] = %f\n",
                       i, this_->so3SensorFromWorld->m[i]);
            }

#endif

            updateCovariancesAfterMotion(this_);
        } else {
            magObservationFunctionForNumericalJacobian(this_->so3SensorFromWorld,
                    this_->mNu, this_);
            So3Util_sO3FromMu(this_->mNu, this_->so3LastMotion);
            Matrix3x3d_mult_M(this_->so3LastMotion, this_->so3SensorFromWorld,
                              this_->processMagTempM4);
            Matrix3x3d_set_o(this_->processMagTempM4, this_->so3SensorFromWorld);

#ifdef DEBUG

            for (int i = 0; i < 9; i++) {
                printf("processMag so3SensorFromWorld[2] this.so3SensorFromWorld.m[%d] =%f\n",
                       i, this_->so3SensorFromWorld->m[i]);
            }

#endif

            updateCovariancesAfterMotion(this_);
            this_->alignedToNorth = true;
        }
    }
}

float* glMatrixFromSo3(Matrix3x3d* so3, OrientationEKF* this_)
{
    /*TODO*/
    int r, c;

    for (r = 0; r < 3; ++r) {
        for (c = 0; c < 3; ++c) {
            this_->rotationMatrix[4 * c + r] = Matrix3x3d_get(r, c, so3);
        }
    }

    this_->rotationMatrix[3] = this_->rotationMatrix[7] = this_->rotationMatrix[11]
                               = 0.0;

    this_->rotationMatrix[12] = this_->rotationMatrix[13] =
                                    this_->rotationMatrix[14] =
                                        0.0;
    this_->rotationMatrix[15] = 1.0;

    return this_->rotationMatrix;
}

void filterGyroTimestep(float timeStep, OrientationEKF* this_)
{
    /*TODO*/

    if (!this_->timestepFilterInit) {
        this_->filteredGyroTimestep = timeStep;
        this_->numGyroTimestepSamples = 1;
        this_->timestepFilterInit = true;
    } else {
        this_->filteredGyroTimestep = 0.95F * this_->filteredGyroTimestep +
                                      0.050000012F *
                                      timeStep;

        if ((float)(++this_->numGyroTimestepSamples) > 10.0F) {
            this_->gyroFilterValid = true;
        }
    }
}

void updateCovariancesAfterMotion(OrientationEKF* this_)
{
    /*TODO*/
    Matrix3x3d_transpose_r(this_->updateCovariancesAfterMotionTempM1,
                           this_->so3LastMotion);
    Matrix3x3d_mult_M(this_->mP, this_->updateCovariancesAfterMotionTempM1,
                      this_->updateCovariancesAfterMotionTempM2);
    Matrix3x3d_mult_M(this_->so3LastMotion,
                      this_->updateCovariancesAfterMotionTempM2,
                      this_->mP);

#ifdef DEBUG

    for (int i = 0; i < 9; i++) {
        printf("updateCovariancesAfterMotion mP.m[%d]=%f\n", i,  this_->mP->m[i]);
    }

#endif

    Matrix3x3d_setIdentity(this_->so3LastMotion);
}

void accObservationFunctionForNumericalJacobian(
    Matrix3x3d* so3SensorFromWorldPred, Vector3d* result, OrientationEKF* this_)
{
    /*TODO*/
    Matrix3x3d_mult_V(so3SensorFromWorldPred, this_->down, this_->mh);
    So3Util_sO3FromTwoVec(this_->mh, this_->mz,
                          this_->accObservationFunctionForNumericalJacobianTempM);
    So3Util_muFromSO3(this_->accObservationFunctionForNumericalJacobianTempM,
                      result);
}

void magObservationFunctionForNumericalJacobian(
    Matrix3x3d* so3SensorFromWorldPred, Vector3d* result, OrientationEKF* this_)
{
    /*TODO*/
    Matrix3x3d_mult_V(so3SensorFromWorldPred, this_->north, this_->mh);
    So3Util_sO3FromTwoVec(this_->mh, this_->mz,
                          this_->magObservationFunctionForNumericalJacobianTempM);
    So3Util_muFromSO3(this_->magObservationFunctionForNumericalJacobianTempM,
                      result);
}
