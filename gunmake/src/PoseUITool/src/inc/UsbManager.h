#include <sys/types.h>

#ifndef USB_MANAGER_H
#define USB_MANAGER_H

struct libusb_device_handle;
struct libusb_device;
class MVRDeviceDriverBase;

class UsbManager
{
public:
    UsbManager();
    ~UsbManager();

    int initUsbDevice();
    libusb_device* findTarget(libusb_device** list, ssize_t cnt);
    void printdev(libusb_device* dev);
    void printDeviceClass(int cls);
    void registerDriver(MVRDeviceDriverBase* driver);
    void unregisterDriver(MVRDeviceDriverBase* driver);
    void readUsbPacket(unsigned char raw[]);

public:
    //static const int mUSB_PRODUCT_ID = 0x5740;
    //static const int mUSB_VENDOR_ID  = 0x0483;
    static const int mUSB_PRODUCT_ID = 0x0308;
    static const int mUSB_VENDOR_ID  = 0x0BB4;

private:
    struct libusb_device_handle* mDevH;
    struct libusb_device** mDevs;
    struct libusb_context* mCtx;
    int mUsbLength;
    MVRDeviceDriverBase* mDriver;
};

#endif // USB_MANAGER_H
