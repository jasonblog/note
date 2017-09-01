#ifndef SENSOREVENT_H
#define SENSOREVENT_H
typedef struct SensorEvent_ {
    unsigned char sensorType;
    int accuracy;
    long long timestamp;
    float values[3];
} SensorEvent;
#endif /*SENSOREVENT_H*/
