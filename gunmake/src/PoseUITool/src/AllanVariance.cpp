#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <inc/AllanVariance.h>
#include <inc/Debug.h>

#include <gnuplot-iostream/gnuplot-iostream.h>

using namespace std;

#define SENSORS 6
#define EPSILON 0.0000001

inline double theta(double val1, double val2, double val3)
{
    return pow((val1 + val2 - 2 * val3), 2);
}

inline double sigma2(double theta, int N, double m, double t0)
{
    return theta / (2 * pow(m * t0, 2) * (N - 2 * m));
}

#define EPSILON 0.0000001
static inline void updateBound(double& MAX, double& MIN, double val)
{
    if (val >= MAX) {
        MAX = val;
    }

    if (val <= MIN) {
        MIN = val;
    }
}

void AllanVariance::appendFrame(double accx, double accy,
                                double accz, double gyrox, double gyroy, double gyroz)
{
    ax.push_back(accx);
    ay.push_back(accy);
    az.push_back(accz);
    gx.push_back(gyrox);
    gy.push_back(gyroy);
    gz.push_back(gyroz);
}

void AllanVariance::computeAllanVarianceFromFile(char path[])
{
    ifstream ifs(path);
    string line, token;
    int idx = 0;
    int i = 0;

    if (ifs.is_open()) {
        while (getline(ifs, line)) {
            istringstream in(line);

            while (getline(in, token, ',')) {
#ifdef DEBUG
                cout << atof(token.c_str()) << " ";
#endif //DEBUG

                switch (i) {
                case 0:
                    ax.push_back(atof(token.c_str()));
                    break;

                case 1:
                    ay.push_back(atof(token.c_str()));
                    break;

                case 2:
                    az.push_back(atof(token.c_str()));
                    break;

#if (SENSORS == 6)

                case 3:
                    gx.push_back(atof(token.c_str()));
                    break;

                case 4:
                    gy.push_back(atof(token.c_str()));
                    break;

                case 5:
                    gz.push_back(atof(token.c_str()));
                    break;
#endif // SENSORS == 6

                default:
                    cout << "Error\n";
                    break;
                }

                if (++i == SENSORS) {
                    i = 0;
#ifdef DEBUG
                    cout << "\n";
#endif //DEBUG
                }
            }
        }

        ifs.close();
    }

    computeAllanVariance();
}

void AllanVariance::computeAllanVariance()
{
    ssize_t N = ax.size();
    ssize_t maxN = pow(2, floor(log2(N / 2)));
    double endLogInc = log10(maxN);
    double t0 = 1 / mFPS;
    cout << "**********Allan Variance Configuration************\n";
    cout << "Sensor Data Type: " << SENSORS << endl;
    cout << "N: " << N << "\tmaxN: " << maxN << "\tendLogInc: " <<
         ceil(endLogInc) << endl;
    cout << "t0: " << t0 << endl;
    cout << "**************************************************\n";
    double sumax = 0.0f;
    double sumay = 0.0f;
    double sumaz = 0.0f;
    double sumgx = 0.0f;
    double sumgy = 0.0f;
    double sumgz = 0.0f;

    double biax = numeric_limits<double>::max();
    double biay = numeric_limits<double>::max();
    double biaz = numeric_limits<double>::max();
    double bigx = numeric_limits<double>::max();
    double bigy = numeric_limits<double>::max();
    double bigz = numeric_limits<double>::max();

    double* handleax = ax.data();
    double* handleay = ay.data();
    double* handleaz = az.data();
    double* handlegx = gx.data();
    double* handlegy = gy.data();
    double* handlegz = gz.data();
    double sigma = 0.0f;
    vector<pair<double, double> > raw_pts_ax;
    vector<pair<double, double> > raw_pts_ay;
    vector<pair<double, double> > raw_pts_az;
    vector<pair<double, double> > raw_pts_gx;
    vector<pair<double, double> > raw_pts_gy;
    vector<pair<double, double> > raw_pts_gz;
    Gnuplot gp;
    double MAX = 0.0;
    double MIN = numeric_limits<double>::max();
    double MAXx = 0.0;
    double MINx = numeric_limits<double>::max();

    for (int m = 1; m < maxN / 2; m += ceil(endLogInc)) {
        for (int k = 0; k < N - 2 * m; k++) {
#ifdef DEBUG
            cout << m << "[" << k << "]: " << handleax[k] << " [" << k + 2 * m << "]: " <<
                 handleax[k + 2 * m] << "["
                 << k + m << "]: " << handleax[k + m] << " ";

            cout << m << "[" << k << "]: " << handlegx[k] << " [" << k + 2 * m << "]: " <<
                 handlegx[k + 2 * m] << "["
                 << k + m << "]: " << handlegx[k + m] << " ";
#endif //DEBUG
            sumax += theta(handleax[k], handleax[k + 2 * m], handleax[k + m]);
            sumay += theta(handleay[k], handleay[k + 2 * m], handleay[k + m]);
            sumaz += theta(handleaz[k], handleaz[k + 2 * m], handleaz[k + m]);
#if (SENSORS == 6)
            sumgx += theta(handlegx[k], handlegx[k + 2 * m], handlegx[k + m]);
            sumgy += theta(handlegy[k], handlegy[k + 2 * m], handlegy[k + m]);
            sumgz += theta(handlegz[k], handlegz[k + 2 * m], handlegz[k + m]);
#endif // SENSORS == 6
#ifdef DEBUG
            cout << sumax << endl;
            cout << sumay << endl;
            cout << sumaz << endl;
#if (SENSORS == 6)
            cout << sumgx << endl;
            cout << sumgy << endl;
            cout << sumgz << endl;
#endif // SENSORS == 6
#endif //DEBUG
        }

        mT.push_back(m * t0);
        updateBound(MAXx, MINx, (m * t0));

        sigma = sqrt(sigma2(sumax, maxN, m, t0));
        raw_pts_ax.push_back(make_pair((m * t0), (sigma))); //prepare for gnuplot
        updateBound(MAX, MIN, (sigma));

        if (sigma < biax) {
            biax = sigma;
        }

        sigma = sqrt(sigma2(sumay, maxN, m, t0));
        raw_pts_ay.push_back(make_pair((m * t0), (sigma))); //prepare for gnuplot
        updateBound(MAX, MIN, (sigma));

        if (sigma < biay) {
            biay = sigma;
        }

        sigma = sqrt(sigma2(sumaz, maxN, m, t0));
        raw_pts_az.push_back(make_pair((m * t0), (sigma))); //prepare for gnuplot
        updateBound(MAX, MIN, (sigma));

        if (sigma < biaz) {
            biaz = sigma;
        }

#if (SENSORS == 6)
        sigma = sqrt(sigma2(sumgx, maxN, m, t0));
        raw_pts_gx.push_back(make_pair((m * t0), (sigma))); //prepare for gnuplot
        updateBound(MAX, MIN, (sigma));

        if (sigma < bigx) {
            bigx = sigma;
        }

        sigma = sqrt(sigma2(sumgy, maxN, m, t0));
        raw_pts_gy.push_back(make_pair((m * t0), (sigma))); //prepare for gnuplot
        updateBound(MAX, MIN, (sigma));

        if (sigma < bigy) {
            bigy = sigma;
        }

        sigma = sqrt(sigma2(sumgz, maxN, m, t0));
        raw_pts_gz.push_back(make_pair((m * t0), (sigma))); //prepare for gnuplot
        updateBound(MAX, MIN, (sigma));

        if (sigma < bigz) {
            bigz = sigma;
        }

#endif // SENSORS == 6
        sumax = 0.0f;
        sumgx = 0.0f;
        sumgy = 0.0f;
        sumgz = 0.0f;
    }

    //Gnuplot for AD
    gp << "set logscale xy\n" << endl;
    gp << "set xrange [" << MINx << ":" << MAXx << "]\nset yrange [" << MIN << ":"
       << MAX << "]\n";
#if (SENSORS == 6)
    gp << "plot '-' lc rgb 'purple' with lines title 'AccelX', '-' lc rgb 'yellow' with lines title 'AccelY', '-' lc rgb 'red' with lines title 'AccelZ', '-' lc rgb 'black' with lines title 'GyroX', '-' lc rgb 'green' with lines title 'GyroY', '-' lc rgb 'blue' with lines title 'GyroZ'\n";
#else
    gp << "plot '-' lc rgb 'purple' with lines title 'AccelX', '-' lc rgb 'yellow' with lines title 'AccelY', '-' lc rgb 'red' with lines title 'AccelZ'\n";
#endif // SENSORS == 6

    gp.send1d(raw_pts_ax);
    gp.send1d(raw_pts_ay);
    gp.send1d(raw_pts_az);
#if (SENSORS == 6)
    gp.send1d(raw_pts_gx);
    gp.send1d(raw_pts_gy);
    gp.send1d(raw_pts_gz);
#endif // SENSORS == 6

#ifdef DEBUG
    cout << biax << " " << biay << " " << biaz << " " << bigx << " " << bigy << " "
         << bigz << endl;
#endif //DEBUG
    mBI.push_back(biax);
    mBI.push_back(biay);
    mBI.push_back(biaz);
    mBI.push_back(bigx);
    mBI.push_back(bigy);
    mBI.push_back(bigz);
}

void AllanVariance::getBiasInstability()
{
    for (vector<double>::iterator it = mBI.begin(); it != mBI.end(); ++it) {
        cout << *it << " ";
    }

    cout << endl;
}

void AllanVariance::getAngularWhiteBias()
{
}
