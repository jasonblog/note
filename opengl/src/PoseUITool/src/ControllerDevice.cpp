#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <inc/utils/Quaternion.h>

#include <inc/ControllerDevice.h>

#define RAWDATASIZE 16

using namespace std;

ControllerDevice::ControllerDevice()
{
}

ControllerDevice::~ControllerDevice()
{
}

void ControllerDevice::parseUsbPacket(unsigned char raw[], Quaternion& q,
                                      double sensors[])
{
    readPacketFromFile(raw);

    short tmpQ_w = (short)(((raw[1] & 0xFF) << 8) | (raw[0] & 0xFF)); //w
    short tmpQ_x = (short)(((raw[3] & 0xFF) << 8) | (raw[2] & 0xFF)); //x
    short tmpQ_y = (short)(((raw[5] & 0xFF) << 8) | (raw[4] & 0xFF)); //y
    short tmpQ_z = (short)(((raw[7] & 0xFF) << 8) | (raw[6] & 0xFF)); //z
    //long serial = (long) translate(raw, 20, 4);
    //long long timeStamp = translate(raw, 24, 8);

    // Parse Gyro data
    //short tmpG_x = (short)( ((raw[15]&0xFF)<<8) | (raw[14]&0xFF)); //x
    //short tmpG_y = (short)( ((raw[17]&0xFF)<<8) | (raw[16]&0xFF)); //y
    //short tmpG_z = (short)( ((raw[19]&0xFF)<<8) | (raw[18]&0xFF)); //z

    // Parse GSensor data
    //short tmpA_x = (short)( ((raw[9]&0xFF)<<8) | (raw[8]&0xFF));     //x
    //short tmpA_y = (short)( ((raw[11]&0xFF)<<8) | (raw[10]&0xFF));   //y
    //short tmpA_z = (short)( ((raw[13]&0xFF)<<8) | (raw[12]&0xFF));   //z

    double qw = QFormatShortToDouble(tmpQ_w, 14);
    double qx = QFormatShortToDouble(tmpQ_x, 14);
    double qy = QFormatShortToDouble(tmpQ_y, 14);
    double qz = QFormatShortToDouble(tmpQ_z, 14);

    //double gx = QFormatShortToDouble(tmpG_x, 9);
    //double gy = QFormatShortToDouble(tmpG_y, 9);
    //double gz = QFormatShortToDouble(tmpG_z, 9);

    //double ax = QFormatShortToDouble(tmpA_x, 9);
    //double ay = QFormatShortToDouble(tmpA_y, 9);
    //double az = QFormatShortToDouble(tmpA_z, 9);

    //cout << "Quat(" << qw << ", " << qx << ", " << qy << ", " << qz << ")" << endl;

    q.w = qw;
    q.x = qx;
    q.y = qy;
    q.z = qz;
}

long long ControllerDevice::translate(unsigned char raw[], int start,
                                      int length)
{
    long long ret = 0;

    for (int i = length - 1; i >= 0; i--) {
        ret *= 256;
        ret += raw[start + i];
    }

    return ret;
}

void ControllerDevice::readPacketFromFile(unsigned char raw[])
{
    unsigned char data[RAWDATASIZE];
    unsigned int size = 0;
    int fd;

    fd = open("./lastQuaternion.log", O_RDONLY);

    if (fd < 0) {
        cout << "Open Error! Check if the file is exist and you have the permission!" <<
             endl;
        return;
    } else {
        do {
            size = read(fd, data, RAWDATASIZE);

            if (size == RAWDATASIZE) {
                break;
            }
        } while (1);
    }

    for (unsigned int i = 0; i < RAWDATASIZE; i++) {
        if (data[i] >= 48 && data[i] <= 57) {
            // handle 0~9
            data[i] -= 48;
        } else if (data[i] >= 97 && data[i] <= 102) {
            // handle a~f
            data[i] -= 87;
        } else {
            cout << "Read Error! Data is wrong!" << endl;
            return;
        }
    }

    for (unsigned int i = 0; i < RAWDATASIZE / 2; i++) {
        raw[i] = (data[2 * i] << 4) | data[2 * i + 1];
    }

    close(fd);
}
