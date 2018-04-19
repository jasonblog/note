
#ifndef CONTROLLER_DEVICE_H
#define CONTROLLER_DEVICE_H

class Quaternion;

class ControllerDevice
{
public:
    ControllerDevice();
    ~ControllerDevice();

    void parseUsbPacket(unsigned char raw[], Quaternion& q, double sensors[]);
    static double QFormatShortToDouble(short x, int n)
    {
        return (((double)x) / ((double)(1 << n)));
    }
    long long translate(unsigned char raw[], int start, int length);

private:
    void readPacketFromFile(unsigned char raw[]);
};

#endif // CONTROLLER_DEVICE_H
