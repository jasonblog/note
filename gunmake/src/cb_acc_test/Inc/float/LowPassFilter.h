#ifndef LOWPASSFILTER_H
#define LOWPASSFILTER_H
#include "Vector3d.h"

typedef struct LowPassFilter_ {
    float NANOS_TO_SECONDS;
    float timeConstantSecs;
    long long lastTimestampNs;
    int numSamples;
    Vector3d* filteredData;
    Vector3d Instance_filteredData;
    Vector3d* temp;
    Vector3d Instance_temp;
} LowPassFilter;

void new_LowPassFilter(float cutoffFrequency, LowPassFilter* this_);
int getNumSamples(LowPassFilter* this_);
void addSample(Vector3d* sampleData, long long timestampNs,
               LowPassFilter* this_);
void addWeightedSample(Vector3d* sampleData, long long timestampNs,
                       float weight, LowPassFilter* this_);
Vector3d* getFilteredData(LowPassFilter* this_);
#endif /* LOWPASSFILTER_H */
