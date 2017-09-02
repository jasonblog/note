#include <stdio.h>
#include "../Inc/float/LowPassFilter.h"

void new_LowPassFilter(float cutoffFrequency, LowPassFilter* this_)
{
    this_->NANOS_TO_SECONDS = 1.0; /*1.0 / TimeUnit.NANOSECONDS TODO*/
    this_->timeConstantSecs = 1.0 / (6.283185307179586 * cutoffFrequency);
    this_->lastTimestampNs = 0;
    this_->numSamples = 0;

    this_->filteredData = &this_->Instance_filteredData;
    new_Vector3d(this_->filteredData);

    this_->temp = &this_->Instance_temp;
    new_Vector3d(this_->temp);
}

int getNumSamples(LowPassFilter* this_)
{
    return this_->numSamples;
}

void addSample(Vector3d* sampleData, long long timestampNs,
               LowPassFilter* this_)
{
    addWeightedSample(sampleData, timestampNs, 1.0, this_);
}

void addWeightedSample(Vector3d* sampleData, long long timestampNs,
                       float weight, LowPassFilter* this_)
{
    /*TODO*/
    ++this_->numSamples;

#ifdef DEBUG
    printf("addWeightedSample numSamples=%d\n", this_->numSamples);
#endif

    if (this_->numSamples == 1) {
        Vector3d_set_o(sampleData, this_->filteredData);

#ifdef DEBUG
        printf("addWeightedSample set filteredData x=%f, y=%f, z=%f\n",
               this_->filteredData->x,
               this_->filteredData->y,
               this_->filteredData->z);
#endif

        this_->lastTimestampNs = timestampNs;
    } else {
        /*NANOS_TO_SECONDS = 1.0D / (float)TimeUnit.NANOSECONDS.convert(1L, TimeUnit.SECONDS); TODO*/
        this_->NANOS_TO_SECONDS = 1.0E-9; /*TODO*/

        float weightedDeltaSecs = weight * (float)(timestampNs -
                                  this_->lastTimestampNs)
                                  * this_->NANOS_TO_SECONDS;

        float alpha = weightedDeltaSecs / (this_->timeConstantSecs +
                                           weightedDeltaSecs);

#ifdef DEBUG
        printf("addWeightedSample alpha=%f\n", alpha);
#endif

        Vector3d_scale(1.0 - alpha, this_->filteredData);
        Vector3d_set_o(sampleData, this_->temp);
        Vector3d_scale(alpha, this_->temp);
        Vector3d_add(this_->temp, this_->filteredData, this_->filteredData);

#ifdef DEBUG
        printf("addWeightedSample add filteredData x=%f, y=%f, z=%f\n",
               this_->filteredData->x,
               this_->filteredData->y,
               this_->filteredData->z);
#endif

        this_->lastTimestampNs = timestampNs;
    }
}

Vector3d* getFilteredData(LowPassFilter* this_)
{
    /*TODO*/
    return this_->filteredData;
}
