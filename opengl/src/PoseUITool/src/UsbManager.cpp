#include <iostream>
#include <libusb.h>
#include <inc/UsbManager.h>
#include <inc/MVRDeviceDriverBase.h>

using namespace std;

UsbManager::UsbManager()
    : mDevH(NULL), mDriver(NULL)
{
}

UsbManager::~UsbManager()
{
    libusb_free_device_list(mDevs, 1); //free the list, unref the devices in it
    libusb_exit(mCtx); //close the session

    if (mDevH != NULL) {
        mDevH = NULL;
    }
}

int UsbManager::initUsbDevice()
{
    int r; //for return values
    ssize_t cnt; //holding number of devices in list
    libusb_device* target = NULL;

    r = libusb_init(&mCtx); //initialize a library session

    if (r < 0) {
        cout << "Init Error " << r << endl; //there was an error
        return 1;
    }

    //libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation

    cnt = libusb_get_device_list(mCtx, &mDevs); //get the list of devices

    if (cnt < 0) {
        cout << "Get Device Error" << endl; //there was an error
    }

    cout << cnt << " Devices in list." << endl; //print total number of usb devices}

    target = findTarget(mDevs, cnt);

    if (target == NULL) {
        cout << "Target not found !!" << endl;
    } else {
        mDevH = libusb_open_device_with_vid_pid(mCtx, mUSB_VENDOR_ID, mUSB_PRODUCT_ID);

        if (libusb_kernel_driver_active(mDevH, 0)) {
            libusb_detach_kernel_driver(mDevH, 0);
        }

        if (mDevH > 0) {
            mUsbLength = libusb_get_max_packet_size(libusb_get_device(mDevH), 0x81);
            printdev(libusb_get_device(mDevH));
            cout << "length: " << mUsbLength << endl;
            int r = libusb_claim_interface(mDevH, 0);

        } else {
            cout << "Open target device failed" << endl;
        }
    }

    return 0;
}

libusb_device* UsbManager::findTarget(libusb_device** list, ssize_t cnt)
{
    ssize_t i;
    libusb_device_descriptor desc;
    int r;

    //cout << answer[1] << answer[2] << answer[3] << answer[4] << endl;
    for (i = 0; i < cnt; i++) {
        r = libusb_get_device_descriptor(list[i], &desc);

        if (r < 0) {
            continue;
        }

        if ((desc.idVendor == mUSB_VENDOR_ID) && (desc.idProduct == mUSB_PRODUCT_ID)) {
            cout << "found target at " << i << endl;
            break;
        }
    }

    return list[i];
}

void UsbManager::printdev(libusb_device* dev)
{
    libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);

    if (r < 0) {
        cout << "failed to get device descriptor" << endl;
        return;
    }

    cout << "VendorID: 0x" << std::hex << desc.idVendor << "  ";
    cout << "ProductID: 0x" << desc.idProduct << endl;
    cout << std::dec;
    cout << "Number of possible configurations: " << (int)desc.bNumConfigurations <<
         "  ";
    cout << "Device Class: ";
    printDeviceClass((int)desc.bDeviceClass);
    libusb_config_descriptor* config;
    libusb_get_config_descriptor(dev, 0, &config);
    cout << "Interfaces: " << (int)config->bNumInterfaces << " ||| ";
    const libusb_interface* inter;
    const libusb_interface_descriptor* interdesc;
    const libusb_endpoint_descriptor* epdesc;

    for (int i = 0; i < (int)config->bNumInterfaces; i++) {
        inter = &config->interface[i];
        cout << "Number of alternate settings: " << inter->num_altsetting << " | ";

        for (int j = 0; j < inter->num_altsetting; j++) {
            interdesc = &inter->altsetting[j];
            cout << "Interface Number: " << (int)interdesc->bInterfaceNumber << " | ";
            cout << "Number of endpoints: " << (int)interdesc->bNumEndpoints << " | ";

            for (int k = 0; k < (int)interdesc->bNumEndpoints; k++) {
                epdesc = &interdesc->endpoint[k];
                cout << "Descriptor Type: " << (int)epdesc->bDescriptorType << " | ";
                cout << "EP Address: " << (int)epdesc->bEndpointAddress << " | ";
            }
        }
    }

    cout << endl << endl << endl;
    libusb_free_config_descriptor(config);
}

void UsbManager::printDeviceClass(int cls)
{
    switch (cls) {
    case LIBUSB_CLASS_PER_INTERFACE:
        cout << "LIBUSB_CLASS_PER_INTERFACE ";
        break;

    case LIBUSB_CLASS_AUDIO:
        cout << "LIBUSB_CLASS_AUDIO ";
        break;

    case LIBUSB_CLASS_COMM:
        cout << "LIBUSB_CLASS_COMM ";
        break;

    case LIBUSB_CLASS_HID:
        cout << "LIBUSB_CLASS_HID ";
        break;

    case LIBUSB_CLASS_PHYSICAL:
        cout << "LIBUSB_CLASS_PHYSICAL ";
        break;

    case LIBUSB_CLASS_PRINTER:
        cout << "LIBUSB_CLASS_PRINTER ";
        break;

    case LIBUSB_CLASS_PTP:
        cout << "LIBUSB_CLASS_PTP|LIBUSB_CLASS_IMAGE ";
        break;

    case LIBUSB_CLASS_MASS_STORAGE:
        cout << "LIBUSB_CLASS_MASS_STORAGE ";
        break;

    case LIBUSB_CLASS_HUB:
        cout << "LIBUSB_CLASS_HUB ";
        break;

    case LIBUSB_CLASS_DATA:
        cout << "LIBUSB_CLASS_DATA ";
        break;

    case LIBUSB_CLASS_SMART_CARD:
        cout << "LIBUSB_CLASS_SMART_CARD ";
        break;

    case LIBUSB_CLASS_CONTENT_SECURITY:
        cout << "LIBUSB_CLASS_CONTENT_SECURITY ";
        break;

    case LIBUSB_CLASS_VIDEO:
        cout << "LIBUSB_CLASS_VIDEO ";
        break;

    case LIBUSB_CLASS_PERSONAL_HEALTHCARE:
        cout << "LIBUSB_CLASS_PERSONAL_HEALTHCARE ";
        break;

    case LIBUSB_CLASS_DIAGNOSTIC_DEVICE:
        cout << "LIBUSB_CLASS_DIAGNOSTIC_DEVICE ";
        break;

    case LIBUSB_CLASS_WIRELESS:
        cout << "LIBUSB_CLASS_WIRELESS ";
        break;

    case LIBUSB_CLASS_APPLICATION:
        cout << "LIBUSB_CLASS_APPLICATION ";
        break;

    case LIBUSB_CLASS_VENDOR_SPEC:
        cout << "LIBUSB_CLASS_VENDOR_SPEC ";
        break;

    default:
        cout << "Unknown class ";
        break;
    }
}

void UsbManager::registerDriver(MVRDeviceDriverBase* driver)
{
    if (mDriver != NULL) {
        cout << "Already reigster driver!!\n";
    } else {
        mDriver = driver;
    }
}

void UsbManager::unregisterDriver(MVRDeviceDriverBase* driver)
{
    if (mDriver != driver) {
        cout << "Don't register " << driver << endl;
    } else {
        mDriver = NULL;
    }
}

void UsbManager::readUsbPacket(unsigned char raw[])
{
    int r, transferred;

    if (mDevH != NULL) {
        r = libusb_interrupt_transfer(mDevH, 0x81, raw, mUsbLength, &transferred, 100);
    }
}
