#include <inc/MVRDeviceDriverBase.h>
#ifndef HMD_DEVICE_H
#define HMD_DEVICE_H

class UsbManager;
class Quaternion;

class HmdDevice : virtual public MVRDeviceDriverBase
{
public:
    HmdDevice(bool isdk1);
    virtual ~HmdDevice();

    virtual void parseUsbPacket(unsigned char raw[], Quaternion& q,
                                double sensors[]);
    static double QFormatShortToDouble(short x, int n)
    {
        return (((double)x) / ((double)(1 << n)));
    }
    long long translate(unsigned char raw[], int start, int length);

private:
    int mIsDK1;
};

#endif // HMD_DEVICE_H
