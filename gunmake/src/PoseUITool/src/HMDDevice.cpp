#include <iostream>
#include <inc/utils/Quaternion.h>
#include <inc/utils/RotationMatrix.h>

#include <inc/HMDDevice.h>

using namespace std;

HmdDevice::HmdDevice(bool isdk1)
    : MVRDeviceDriverBase()
    , mIsDK1(isdk1)
{
    if (!mIsDK1) {
        cout << "TANK connect!!\n";
    } else {
        cout << "DK1 connect!!\n";
    }
}

HmdDevice::~HmdDevice()
{
    mManager = NULL;
}

void HmdDevice::parseUsbPacket(unsigned char raw[], Quaternion& q,
                               double sensors[])
{
    if (mManager != NULL) {
        mManager->readUsbPacket(raw);

        short tmpQ_w = (short)(((raw[1] & 0xFF) << 8) | (raw[0] & 0xFF)); //w
        short tmpQ_x = (short)(((raw[3] & 0xFF) << 8) | (raw[2] & 0xFF)); //x
        short tmpQ_y = (short)(((raw[5] & 0xFF) << 8) | (raw[4] & 0xFF)); //y
        short tmpQ_z = (short)(((raw[7] & 0xFF) << 8) | (raw[6] & 0xFF)); //z
        long serial = (long) translate(raw, 20, 4);
        long long timeStamp = translate(raw, 24, 8);

        // Parse Gyro data
        short tmpG_x = (short)(((raw[15] & 0xFF) << 8) | (raw[14] & 0xFF)); //x
        short tmpG_y = (short)(((raw[17] & 0xFF) << 8) | (raw[16] & 0xFF)); //y
        short tmpG_z = (short)(((raw[19] & 0xFF) << 8) | (raw[18] & 0xFF)); //z

        // Parse GSensor data
        short tmpA_x = (short)(((raw[9] & 0xFF) << 8) | (raw[8] & 0xFF)); //x
        short tmpA_y = (short)(((raw[11] & 0xFF) << 8) | (raw[10] & 0xFF)); //y
        short tmpA_z = (short)(((raw[13] & 0xFF) << 8) | (raw[12] & 0xFF)); //z

        double qw = QFormatShortToDouble(tmpQ_w, 14);
        double qx = QFormatShortToDouble(tmpQ_x, 14);
        double qy = QFormatShortToDouble(tmpQ_y, 14);
        double qz = QFormatShortToDouble(tmpQ_z, 14);

        double gx = QFormatShortToDouble(tmpG_x, 9);
        double gy = QFormatShortToDouble(tmpG_y, 9);
        double gz = QFormatShortToDouble(tmpG_z, 9);

        double ax = QFormatShortToDouble(tmpA_x, 9);
        double ay = QFormatShortToDouble(tmpA_y, 9);
        double az = QFormatShortToDouble(tmpA_z, 9);

#if 0
        cout << "[" << serial << "]; Quat(" << qw << ", " << qx << ", "
             << qy << ", " << qz << "); Acc(" << ax << ", " << ay << ", "
             << az << "); Gyro(" << gx << ", " << gy << ", " << gz << ")\n";
#endif
        q.w = qw;
        q.x = qx;
        q.y = qy;
        q.z = qz;

        if (sensors != NULL) {
            sensors[0] = ax;
            sensors[1] = ay;
            sensors[2] = az;
            sensors[3] = gx;
            sensors[4] = gy;
            sensors[5] = gz;
        }

        //Adjust to World coordinate
        Vector3d euler;

        if (!mIsDK1) {
            euler.setComponent(0, 180.0);
            euler.setComponent(1, 0.0);
            euler.setComponent(2, 180.0);
        } else {
            euler.setComponent(0, 0.0);
            euler.setComponent(1, 90.0);
            euler.setComponent(2, 0.0);
        }

        Quaternion adjust;
        adjust.converEulerTo(euler);
        Matrix3x3d adjustM = adjust.toRotationMatrix();
        Matrix3x3d m = adjustM * q.toRotationMatrix();
        RotationMatrix rm(m.mInstance[0], m.mInstance[1], m.mInstance[2],
                          m.mInstance[3], m.mInstance[4], m.mInstance[5],
                          m.mInstance[6], m.mInstance[7], m.mInstance[8]);
        q = rm.toQuat();
    }
}

long long HmdDevice::translate(unsigned char raw[], int start, int length)
{
    long long ret = 0;

    for (int i = length - 1; i >= 0; i--) {
        ret *= 256;
        ret += raw[start + i];
    }

    return ret;
}
