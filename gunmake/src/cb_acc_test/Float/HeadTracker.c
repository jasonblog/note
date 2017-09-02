#include <stdio.h>
#include "../Inc/float/HeadTracker.h"
#include "../Inc/float/Matrix.h"

HeadTracker HeadTracker_Instance;
HeadTracker* HeadTracker_ = NULL;

void new_HeadTracker(HeadTracker* this_)
{
    this_->gyroBiasEstimator = NULL;
    this_->ekfToHeadTracker = this_->Instance_ekfToHeadTracker;
    this_->sensorToDisplay = this_->Instance_sensorToDisplay;
    this_->neckModelTranslation = this_->Instance_neckModelTranslation;
    this_->tmpHeadView = this_->Instance_tmpHeadView;
    this_->tmpHeadView2 = this_->Instance_tmpHeadView2;
    this_->initialSystemGyroBias = this_->Instance_initialSystemGyroBias;

    this_->tracker = &this_->Instance_OrientationEKF;
    new_OrientationEKF(this_->tracker);

    setGyroBiasEstimationEnabled(true, this_);
    setIdentityM(this_->neckModelTranslation, 0);

    this_->gyroBias = &this_->Instance_gyroBias;
    new_Vector3d(this_->gyroBias);

    this_->latestGyro = &this_->Instance_latestGyro;
    new_Vector3d(this_->latestGyro);

    this_->latestAcc = &this_->Instance_latestAcc;
    new_Vector3d(this_->latestAcc);

    this_->displayRotation = -3.4028235E38f;
    this_->neckModelFactor = 1.0f;
    this_->tracking = false;
    this_->latestGyroEventClockTimeNs = 0;
    this_->firstGyroValue = true;
}

HeadTracker* get_HeadTracker_Instance()
{

    if (HeadTracker_ == NULL) {
        HeadTracker_ = &HeadTracker_Instance;
        new_HeadTracker(HeadTracker_);
    }

    return HeadTracker_;
}

void onSensorChanged(SensorEvent* event, HeadTracker* this_)  /* TODO */
{
    if (event->sensorType == 1) {

        Vector3d_set((float) event->values[0],
                     (float) event->values[1],
                     (float) event->values[2],
                     this_->latestAcc);

#ifdef DEBUG
        printf("onSensorChanged processAcc[+] latestAcc x=%f latestAcc y=%f latestAcc z=%f\n",
               this_->latestAcc->x, this_->latestAcc->y, this_->latestAcc->z);
#endif

        processAcc(this_->latestAcc, event->timestamp, this_->tracker);

#ifdef DEBUG
        printf("onSensorChanged processAcc[-] latestAcc x=%f latestAcc y=%f latestAcc z=%f\n",
               this_->latestAcc->x, this_->latestAcc->y, this_->latestAcc->z);
#endif

        if (this_->gyroBiasEstimator != NULL) {
            processAccelerometer(this_->latestAcc, event->timestamp,
                                 this_->gyroBiasEstimator);

#ifdef DEBUG
            printf("onSensorChanged processAccelerometer latestAcc x=%f latestAcc y=%f latestAcc z=%f\n",
                   this_->latestAcc->x, this_->latestAcc->y, this_->latestAcc->z);
#endif
        }

    } else if (event->sensorType == 4) {
        /*this.latestGyroEventClockTimeNs = this.clock.nanoTime();TODO*/

        Vector3d_set((float) event->values[0],
                     (float) event->values[1],
                     (float) event->values[2],
                     this_->latestGyro);

        this_->firstGyroValue = false;

        if (this_->gyroBiasEstimator != NULL) {
            processGyroscope(this_->latestGyro, event->timestamp, this_->gyroBiasEstimator);
            getGyroBias(this_->gyroBias, this_->gyroBiasEstimator);
            Vector3d_sub(this_->latestGyro, this_->gyroBias, this_->latestGyro);
        }

#ifdef DEBUG
        printf("onSensorChanged processGyro[+] latestGyro x=%f latestGyro y=%f latestGyro z=%f\n",
               this_->latestGyro->x, this_->latestGyro->y, this_->latestGyro->z);
#endif

        processGyro(this_->latestGyro, event->timestamp, this_->tracker);

#ifdef DEBUG
        printf("onSensorChanged processGyro[-] latestGyro x=%f latestGyro y=%f latestGyro z=%f\n",
               this_->latestGyro->x, this_->latestGyro->y, this_->latestGyro->z);
#endif

    } else if (event->sensorType == 2) {

#ifdef DEBUG
        printf("onSensorChanged processMag[+] Mag x=%f Mag y=%f Mag z=%f\n",
               event->values[0],
               event->values[1],
               event->values[2]);
#endif

        processMag(event->values, event->timestamp, this_->tracker);

#ifdef DEBUG
        printf("onSensorChanged processMag[-] Mag x=%f Mag y=%f Mag z=%f\n",
               event->values[0],
               event->values[1],
               event->values[2]);
#endif
    }
}

void onAccuracyChanged(/*Sensor* sensor, */int accuracy,
        HeadTracker* this_) /* TODO */
{

}

void startTracking(HeadTracker* this_)
{
#if 0

    if (!this_->tracking) {
        this.tracker.reset();

        /*Object var1 = this.gyroBiasEstimatorMutex; TODO*/
        /*synchronized(this.gyroBiasEstimatorMutex) {TODO*/
        if (this_->gyroBiasEstimator != null) {
            this.gyroBiasEstimator.reset();
        }

        /*
        }
        */

        this_->firstGyroValue = true;
        /* TODO
        sensorEventProvider.registerListener(this);
        sensorEventProvider.start();
        */
        this_->tracking = true;
    }

#endif
}

void resetTracker(HeadTracker* this_)
{
    /*this.tracker.reset(); TODO*/
}

void stopTracking(HeadTracker* this_)
{
    if (this_->tracking) {
        /* TODO
        sensorEventProvider.unregisterListener(this);
        sensorEventProvider.stop();
        */
        this_->tracking = false;
    }
}

void setNeckModelEnabled(bool enabled, HeadTracker* this_)
{
    if (enabled) {
        setNeckModelFactor(1.0F, this_);
    } else {
        setNeckModelFactor(0.0F, this_);
    }
}

float getNeckModelFactor(HeadTracker* this_)
{
    /*Object var1 = this.neckModelFactorMutex;
    synchronized(this.neckModelFactorMutex) {
    TODO*/
    return this_->neckModelFactor;
    /*TODO
        }
    */
}

void setNeckModelFactor(float factor, HeadTracker* this_)
{
    /*Object var2 = this.neckModelFactorMutex; TODO*/
    /*synchronized(this.neckModelFactorMutex) { TODO*/
    if (factor >= 0.0F && factor <= 1.0F) {
        this_->neckModelFactor = factor;
    } else {
        printf("factor should be within [0.0, 1.0]\n");
    }

    /*TODO
        }
    */
}

void setGyroBiasEstimationEnabled(bool enabled, HeadTracker* this_)
{
    /*Object var2 = this.gyroBiasEstimatorMutex;TODO*/
    /*synchronized(this.gyroBiasEstimatorMutex) {TODO*/
    if (!enabled) {
        this_->gyroBiasEstimator = NULL;
    } else if (this_->gyroBiasEstimator == NULL) {
        this_->gyroBiasEstimator = &this_->Instance_gyroBiasEstimator;
        new_GyroscopeBiasEstimator(this_->gyroBiasEstimator);
    }

    /*
        }
    */
}

bool getGyroBiasEstimationEnabled(HeadTracker* this_)
{
    /*
    Object var1 = this.gyroBiasEstimatorMutex;
    synchronized(this.gyroBiasEstimatorMutex) {
    TODO*/
    return this_->gyroBiasEstimator != NULL;
    /*TODO
    }
    */
}

void getLastHeadView(float headView[], int length, int offset,
                     HeadTracker* this_)
{

    if (offset + 16 > length) {
        printf("Not enough space to write the result\n");
    } else {
        float rotation = 0.0F;

        switch (1/*this.display.getRotation() TODO*/) {
        case 0:
            rotation = 0.0F;
            break;

        case 1:
            rotation = 90.0F;
            break;

        case 2:
            rotation = 180.0F;
            break;

        case 3:
            rotation = 270.0F;
        }

        if (rotation != this_->displayRotation) {
            this_->displayRotation = rotation;
            setRotateEulerM(this_->sensorToDisplay, 0, 0.0F, 0.0F, -rotation);
            setRotateEulerM(this_->ekfToHeadTracker, 0, -90.0F, 0.0F, rotation);
        }

        /*synchronized(this.tracker) {TODO*/
        if (!isReady(this_->tracker)) {
            return;
        }

        float secondsSinceLastGyroEvent =
            0.0; /*(float) TimeUnit.NANOSECONDS.toSeconds(
                                               this.clock.nanoTime() - this.latestGyroEventClockTimeNs); TODO*/

        float secondsToPredictForward = secondsSinceLastGyroEvent +
                                        0.057999998331069946;

        float* mat = getPredictedGLMatrix(secondsToPredictForward, this_->tracker);

        for (int i = 0; i < length; ++i) {
#ifdef NORMAL
            headView[i] = (float) mat[i];
#ifdef DEBUG
            printf("headView[%d] = %f\n", i , headView[i]);
#endif // DEBUG
#else
            this_->tmpHeadView[i] = (float) mat[i];
#endif // NORMAL

#ifndef NORMAL
#ifdef DEBUG
            printf("getLastHeadView getPredictedGLMatrix tmpHeadView[%d] = %f\n", i ,
                   this_->tmpHeadView[i]);
#endif // DEBUG
#endif // NORMAL
        }

        /*}*/

#ifndef NORMAL
        multiplyMM(this_->tmpHeadView2, 0, this_->sensorToDisplay, 0,
                   this_->tmpHeadView, 0);
        multiplyMM(headView, offset, this_->tmpHeadView2, 0, this_->ekfToHeadTracker,
                   0);
        setIdentityM(this_->neckModelTranslation, 0);

        translateM(this_->neckModelTranslation, 0, 0.0F,
                   -this_->neckModelFactor * 0.075F, this_->neckModelFactor * 0.08F);

        multiplyMM(this_->tmpHeadView, 0, this_->neckModelTranslation, 0, headView,
                   offset);

        translateM_l(headView, offset, this_->tmpHeadView, 0, 0.0F,
                     this_->neckModelFactor * 0.075F, 0.0F);
#endif // NORMAL
    }
}

void setGyroBiasEstimator(GyroscopeBiasEstimator* estimator,
                          HeadTracker* this_) /* TODO */
{
    /*Object var2 = this.gyroBiasEstimatorMutex;
    synchronized(this.gyroBiasEstimatorMutex) {
    TODO*/
    this_->gyroBiasEstimator = estimator;
    /*}*/
}
