#include <inc/UsbManager.h>

#ifndef MVR_DEVICE_DRIVER_BASE_H
#define MVR_DEVICE_DRIVER_BASE_H

class Quaternion;

class MVRDeviceDriverBase
{
public:
    MVRDeviceDriverBase() { }
    virtual ~MVRDeviceDriverBase()
    {
        if (mManager != NULL) {
            mManager->unregisterDriver(this);
            mManager == NULL;
        }
    }

    virtual void parseUsbPacket(unsigned char raw[], Quaternion& q,
                                double sensors[]) = 0;
    void init(MVRDeviceDriverBase* drv)
    {
        mManager = new UsbManager();

        if (mManager != NULL) {
            mManager->initUsbDevice();
            mManager->registerDriver(drv);
        }
    }

protected:
    UsbManager* mManager;
};

#endif // MV_DEVICE_DRIVER_BASE_H
