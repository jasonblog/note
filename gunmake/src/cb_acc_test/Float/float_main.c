#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "../Inc/float/SensorEvent.h"
#include "../Inc/float/HeadTracker.h"
#include "../Inc/float/HeadTransform.h"

// 9DOF
// #define NINE_DOF

SensorEvent accelEvent;
SensorEvent gyroEvent;
SensorEvent MagEvent;
HeadTransform Instance_headTransform;

static FILE* fp;
static int g_Flag = 1;

static void QuaternionToEuler(float* data, float* q)
{
    /*
    quaternion[offset + 0] = x;
    quaternion[offset + 1] = y;
    quaternion[offset + 2] = z;
    quaternion[offset + 3] = w;
    */

    // yaw:
    data[0] = (float)atan2(2 * (q[0] * q[1] + q[3] * q[2]),
                           q[3] * q[3] + q[0] * q[0] - q[1] * q[1] - q[2] * q[2]);
    // pitch:
    data[1] = (float)asin(-2 * (q[0] * q[2] - q[3] * q[1]));

    // roll:
    data[2] = (float)atan2(2 * (q[1] * q[2] + q[3] * q[0]),
                           q[3] * q[3] - q[0] * q[0] - q[1] * q[1] + q[2] * q[2]);
}

static void main_onSensorChanged(HeadTracker* headTracker)
{
#ifdef NINE_DOF
    accelEvent.sensorType = 1;
    onSensorChanged(&accelEvent, headTracker);

    MagEvent.sensorType = 2;
    onSensorChanged(&MagEvent, headTracker);

    gyroEvent.sensorType = 4;
    onSensorChanged(&gyroEvent, headTracker);

    fscanf(fp, "%f,%f,%f,%lld,%f,%f,%f,%lld,%f,%f,%f,%lld",
           &accelEvent.values[0], &accelEvent.values[1], &accelEvent.values[2],
           &accelEvent.timestamp,
           &gyroEvent.values[0], &gyroEvent.values[1], &gyroEvent.values[2],
           &gyroEvent.timestamp,
           &MagEvent.values[0], &MagEvent.values[1], &MagEvent.values[2],
           &MagEvent.timestamp);

#else
    accelEvent.sensorType = 1;
    onSensorChanged(&accelEvent, headTracker);

    gyroEvent.sensorType = 4;
    onSensorChanged(&gyroEvent, headTracker);

    fscanf(fp, "%f,%f,%f,%lld,%f,%f,%f,%lld",
           &accelEvent.values[0], &accelEvent.values[1], &accelEvent.values[2],
           &accelEvent.timestamp,
           &gyroEvent.values[0], &gyroEvent.values[1], &gyroEvent.values[2],
           &gyroEvent.timestamp);
#endif

#if 1

#if 0

    switch (count) {
    case 1:
        accelEvent.sensorType = 1;
        accelEvent.values[0] = 0.90000f;
        accelEvent.values[1] = -0.04000f;
        accelEvent.values[2] = 9.59000f;
        accelEvent.timestamp = 887669967101548L;
        onSensorChanged(&accelEvent, headTracker);

        MagEvent.sensorType = 2;
        MagEvent.values[0] = 26.59375f;
        MagEvent.values[1] = -12.19531f;
        MagEvent.values[2] = -112.56250f;
        MagEvent.timestamp = 887669967101548L;
        onSensorChanged(&MagEvent, headTracker);

        gyroEvent.sensorType = 4;
        gyroEvent.values[0] = -0.01000f;
        gyroEvent.values[1] = 0.0f;
        gyroEvent.values[2] = 0.0f;
        gyroEvent.timestamp = 887669967101548L;
        onSensorChanged(&gyroEvent, headTracker);
        break;

    case 2:
        accelEvent.sensorType = 1;
        accelEvent.values[0] = 0.89000f;
        accelEvent.values[1] = -0.20000f;
        accelEvent.values[2] = 9.69000f;
        accelEvent.timestamp = 887669977147193L;
        onSensorChanged(&accelEvent, headTracker);

        MagEvent.sensorType = 2;
        MagEvent.values[0] = 26.59375f;
        MagEvent.values[1] = -12.19531f;
        MagEvent.values[2] = -112.56250f;
        MagEvent.timestamp = 887669977147193L;
        onSensorChanged(&MagEvent, headTracker);

        gyroEvent.sensorType = 4;
        gyroEvent.values[0] = 0.00000f;
        gyroEvent.values[1] = 0.00000f;
        gyroEvent.values[2] = 0.00000f;
        gyroEvent.timestamp = 887669977147193L;
        onSensorChanged(&gyroEvent, headTracker);
        break;

    case 3:
        accelEvent.sensorType = 1;
        accelEvent.values[0] = 0.92000f;
        accelEvent.values[1] = -0.16000f;
        accelEvent.values[2] = 9.71000f;
        accelEvent.timestamp = 887669987192838L;
        onSensorChanged(&accelEvent, headTracker);

        MagEvent.sensorType = 2;
        MagEvent.values[0] = 26.59375f;
        MagEvent.values[1] = -12.19531f;
        MagEvent.values[2] = -112.56250f;
        MagEvent.timestamp = 887669987192838L;
        onSensorChanged(&MagEvent, headTracker);

        gyroEvent.sensorType = 4;
        gyroEvent.values[0] = 0.00000f;
        gyroEvent.values[1] = 0.00000f;
        gyroEvent.values[2] = 0.00000f;
        gyroEvent.timestamp = 887669987192838L;
        onSensorChanged(&gyroEvent, headTracker);
        break;

    case 4:
        accelEvent.sensorType = 1;
        accelEvent.values[0] = 0.87000f;
        accelEvent.values[1] = -0.09000f;
        accelEvent.values[2] = 9.62000f;
        accelEvent.timestamp = 887669997238483L;
        onSensorChanged(&accelEvent, headTracker);

        MagEvent.sensorType = 2;
        MagEvent.values[0] = 26.68750f;
        MagEvent.values[1] = -12.19531f;
        MagEvent.values[2] = -112.56250f;
        MagEvent.timestamp = 887669997238483L;
        onSensorChanged(&MagEvent, headTracker);

        gyroEvent.sensorType = 4;
        gyroEvent.values[0] = -0.01000f;
        gyroEvent.values[1] = 0.00000f;
        gyroEvent.values[2] = 0.00000f;
        gyroEvent.timestamp = 887669997238483L;
        onSensorChanged(&gyroEvent, headTracker);
        break;

    case 5:
        accelEvent.sensorType = 1;
        accelEvent.values[0] = 0.84000f;
        accelEvent.values[1] = -0.08000f;
        accelEvent.values[2] = 9.56000f;
        accelEvent.timestamp = 887670007284128L;
        onSensorChanged(&accelEvent, headTracker);

        MagEvent.sensorType = 2;
        MagEvent.values[0] = 26.68750f;
        MagEvent.values[1] = -12.19531f;
        MagEvent.values[2] = -112.68750f;
        MagEvent.timestamp = 887670007284128L;
        onSensorChanged(&MagEvent, headTracker);

        gyroEvent.sensorType = 4;
        gyroEvent.values[0] = 0.00000f;
        gyroEvent.values[1] = 0.00000f;
        gyroEvent.values[2] = 0.00000f;
        gyroEvent.timestamp = 887670007284128L;
        onSensorChanged(&gyroEvent, headTracker);
        break;

    default:
        accelEvent.sensorType = 1;
        accelEvent.values[0] = 0;
        accelEvent.values[1] = 0;
        accelEvent.values[2] = 0;
        accelEvent.timestamp = 0;
        onSensorChanged(&accelEvent, headTracker);

        MagEvent.sensorType = 2;
        MagEvent.values[0] = 0;
        MagEvent.values[1] = 0;
        MagEvent.values[2] = 0;
        MagEvent.timestamp = 0;
        onSensorChanged(&MagEvent, headTracker);

        gyroEvent.sensorType = 4;
        gyroEvent.values[0] = 0;
        gyroEvent.values[1] = 0;
        gyroEvent.values[2] = 0;
        gyroEvent.timestamp = 0;
        onSensorChanged(&gyroEvent, headTracker);
    }

#endif

#else
    accelEvent.sensorType = 1;
    accelEvent.values[0] = 0.90000f;
    accelEvent.values[1] = -0.04000f;
    accelEvent.values[2] = 9.59000f;
    accelEvent.timestamp = 887669967101548L;
    onSensorChanged(&accelEvent, headTracker);

    MagEvent.sensorType = 2;
    MagEvent.values[0] = 26.59375f;
    MagEvent.values[1] = -12.19531f;
    MagEvent.values[2] = -112.56250f;
    MagEvent.timestamp = 887669967101548L;
    onSensorChanged(&MagEvent, headTracker);

    gyroEvent.sensorType = 4;
    gyroEvent.values[0] = -0.01000f;
    gyroEvent.values[1] = 0.0f;
    gyroEvent.values[2] = 0.0f;
    gyroEvent.timestamp = 887669967101548L;
    onSensorChanged(&gyroEvent, headTracker);
#endif
}

void* ap_thread(void* args)
{
    float eulerAngles[3] = {0.0, 0.0, 0.0};

    HeadTracker* headTracker = get_HeadTracker_Instance();
    HeadTransform* headTransform = &Instance_headTransform;
    new_HeadTransform(headTransform);

#if 1

    while (1) {
        if (g_Flag != 2) {
            continue;
        }

        getLastHeadView(getHeadView_l(headTransform), 16, 0, headTracker);

#ifdef NORMAL
        float headRotation[4];
        getQuaternion(headRotation, 4, 0, headTransform);
        QuaternionToEuler(eulerAngles, headRotation);

        printf("yaw = %.5f, pitch = %.5f, roll = %.5f\n",
               ((eulerAngles[0] * 180.0) / 3.14),
               ((eulerAngles[1] * 180.0) / 3.14),
               ((eulerAngles[2] * 180.0) / 3.14));
#else
        getEulerAngles(eulerAngles, 3, 0 , headTransform);

        printf("pitch = %.5f, yaw = %.5f, roll = %.5f\n",
               ((eulerAngles[0] * 180.0) / 3.14),
               ((eulerAngles[1] * 180.0) / 3.14),
               ((eulerAngles[2] * 180.0) / 3.14));
#endif // NORMAL


        fflush(stdout);
        g_Flag = 1;
    }

#else
    usleep(100);
    getLastHeadView(getHeadView_l(headTransform), 16, 0, headTracker);
    getEulerAngles(eulerAngles, 3, 0 , headTransform);

    printf("pitch = %.5f, yaw = %.5f, roll = %.5f\n",
           ((eulerAngles[0] * 180.0) / 3.14),
           ((eulerAngles[1] * 180.0) / 3.14),
           ((eulerAngles[2] * 180.0) / 3.14));

    usleep(100);
    getLastHeadView(getHeadView_l(headTransform), 16, 0, headTracker);
    getEulerAngles(eulerAngles, 3, 0 , headTransform);

    printf("pitch = %.5f, yaw = %.5f, roll = %.5f\n",
           ((eulerAngles[0] * 180.0) / 3.14),
           ((eulerAngles[1] * 180.0) / 3.14),
           ((eulerAngles[2] * 180.0) / 3.14));
#endif

    return NULL;
}

int main(int argc, char* argv[])
{
    pthread_t thread1;
    HeadTracker* headTracker = get_HeadTracker_Instance();

    pthread_create(&thread1, NULL, ap_thread, NULL);

#ifdef NINE_DOF

    if ((fp = fopen("../log/9dof_100.csv", "r")) == NULL) {
        puts("Cannot oepn the file");
        exit(1);
    }

    fscanf(fp, "%f,%f,%f,%lld,%f,%f,%f,%lld,%f,%f,%f,%lld",
           &accelEvent.values[0], &accelEvent.values[1], &accelEvent.values[2],
           &accelEvent.timestamp,
           &gyroEvent.values[0], &gyroEvent.values[1], &gyroEvent.values[2],
           &gyroEvent.timestamp,
           &MagEvent.values[0], &MagEvent.values[1], &MagEvent.values[2],
           &MagEvent.timestamp);
#else

    if ((fp = fopen("6dof_288.csv", "r")) == NULL) {
        puts("Cannot oepn the file");
        exit(1);
    }

    fscanf(fp, "%f,%f,%f,%lld,%f,%f,%f,%lld",
           &accelEvent.values[0], &accelEvent.values[1], &accelEvent.values[2],
           &accelEvent.timestamp,
           &gyroEvent.values[0], &gyroEvent.values[1], &gyroEvent.values[2],
           &gyroEvent.timestamp);
#endif

    while (!feof(fp)) {
        if (g_Flag != 1) {
            continue;
        }

        main_onSensorChanged(headTracker);
        g_Flag = 2;
    }

    pthread_join(thread1, NULL);
    return 0;
}
