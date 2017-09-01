#ifndef ALLAN_VARIANCE_H
#define ALLAN_VARIANCE_H

#include <vector>

class AllanVariance
{
public:
    AllanVariance()
    {
        mFPS = 400.0f;
    }

    ~AllanVariance()
    {
        mT.clear();
    }

    void init()
    {
        mT.clear();
        ax.clear();
        ay.clear();
        az.clear();
        gx.clear();
        gy.clear();
        gz.clear();
        mBI.clear();
        mAWB.clear();
    }

    void computeAllanVarianceFromFile(char path[]);
    void computeAllanVariance();
    void appendFrame(double accx, double accy, double accz,
                     double gyrox, double gyroy, double gyroz);
    void getBiasInstability();
    void getAngularWhiteBias();

private:
    float mFPS;
    std::vector<double> mT;
    std::vector<double> ax;
    std::vector<double> ay;
    std::vector<double> az;
    std::vector<double> gx;
    std::vector<double> gy;
    std::vector<double> gz;
    std::vector<double> mBI;
    std::vector<double> mAWB;
};

#endif //ALLAN_VARIANCE 
