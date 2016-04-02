# kalman filter


```c
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;

unsigned long now, lastTime = 0;
float dt;                                   //微分時間

int16_t ax, ay, az, gx, gy, gz;             //加速度計陀螺儀原始數據
float aax=0, aay=0,aaz=0, agx=0, agy=0, agz=0;    //角度變量
long axo = 0, ayo = 0, azo = 0;             //加速度計偏移量
long gxo = 0, gyo = 0, gzo = 0;             //陀螺儀偏移量

float pi = 3.1415926;
float AcceRatio = 16384.0;                  //加速度計比例係數
float GyroRatio = 131.0;                    //陀螺儀比例係數

uint8_t n_sample = 8;                       //加速度計濾波算法採樣個數
float aaxs[8] = {0}, aays[8] = {0}, aazs[8] = {0};         //x,y軸採樣隊列
long aax_sum, aay_sum,aaz_sum;                      //x,y軸採樣和

float a_x[10]={0}, a_y[10]={0},a_z[10]={0} ,g_x[10]={0} ,g_y[10]={0},g_z[10]={0}; //加速度計協方差計算隊列
float Px=1, Rx, Kx, Sx, Vx, Qx;             //x軸卡爾曼變量
float Py=1, Ry, Ky, Sy, Vy, Qy;             //y軸卡爾曼變量
float Pz=1, Rz, Kz, Sz, Vz, Qz;             //z軸卡爾曼變量

void setup()
{
    Wire.begin();
    Serial.begin(115200);

    accelgyro.initialize();                 //初始化

    unsigned short times = 200;             //採樣次數
    for(int i=0;i<times;i++)
    {
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //讀取六軸原始數值
        axo += ax; ayo += ay; azo += az;      //採樣和
        gxo += gx; gyo += gy; gzo += gz;
    
    }
    
    axo /= times; ayo /= times; azo /= times; //計算加速度計偏移
    gxo /= times; gyo /= times; gzo /= times; //計算陀螺儀偏移
}

void loop()
{
    unsigned long now = millis();             //當前時間(ms)
    dt = (now - lastTime) / 1000.0;           //微分時間(s)
    lastTime = now;                           //上一次採樣時間(ms)

    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //讀取六軸原始數值

    float accx = ax / AcceRatio;              //x軸加速度
    float accy = ay / AcceRatio;              //y軸加速度
    float accz = az / AcceRatio;              //z軸加速度

    aax = atan(accy / accz) * (-180) / pi;    //y軸對於z軸的夾角
    aay = atan(accx / accz) * 180 / pi;       //x軸對於z軸的夾角
    aaz = atan(accz / accy) * 180 / pi;       //z軸對於y軸的夾角

    aax_sum = 0;                              // 對於加速度計原始數據的滑動加權濾波算法
    aay_sum = 0;
    aaz_sum = 0;
  
    for(int i=1;i<n_sample;i++)
    {
        aaxs[i-1] = aaxs[i];
        aax_sum += aaxs[i] * i;
        aays[i-1] = aays[i];
        aay_sum += aays[i] * i;
        aazs[i-1] = aazs[i];
        aaz_sum += aazs[i] * i;
    
    }
    
    aaxs[n_sample-1] = aax;
    aax_sum += aax * n_sample;
    aax = (aax_sum / (11*n_sample/2.0)) * 9 / 7.0; //角度調幅至0-90°
    aays[n_sample-1] = aay;                        //此處應用實驗法取得合適的係數
    aay_sum += aay * n_sample;                     //本例係數為9/7
    aay = (aay_sum / (11*n_sample/2.0)) * 9 / 7.0;
    aazs[n_sample-1] = aaz; 
    aaz_sum += aaz * n_sample;
    aaz = (aaz_sum / (11*n_sample/2.0)) * 9 / 7.0;

    float gyrox = - (gx-gxo) / GyroRatio * dt; //x軸角速度
    float gyroy = - (gy-gyo) / GyroRatio * dt; //y軸角速度
    float gyroz = - (gz-gzo) / GyroRatio * dt; //z軸角速度
    agx += gyrox;                             //x軸角速度積分
    agy += gyroy;                             //x軸角速度積分
    agz += gyroz;
    
    /* kalman start */
    Sx = 0; Rx = 0;
    Sy = 0; Ry = 0;
    Sz = 0; Rz = 0;
    
    for(int i=1;i<10;i++)
    {                 //測量值平均值運算
        a_x[i-1] = a_x[i];                      //即加速度平均值
        Sx += a_x[i];
        a_y[i-1] = a_y[i];
        Sy += a_y[i];
        a_z[i-1] = a_z[i];
        Sz += a_z[i];
    
    }
    
    a_x[9] = aax;
    Sx += aax;
    Sx /= 10;                                 //x軸加速度平均值
    a_y[9] = aay;
    Sy += aay;
    Sy /= 10;                                 //y軸加速度平均值
    a_z[9] = aaz;
    Sz += aaz;
    Sz /= 10;

    for(int i=0;i<10;i++)
    {
        Rx += sq(a_x[i] - Sx);
        Ry += sq(a_y[i] - Sy);
        Rz += sq(a_z[i] - Sz);
    
    }
    
    Rx = Rx / 9;                              //得到方差
    Ry = Ry / 9;                        
    Rz = Rz / 9;
  
    Px = Px + 0.0025;                         // 0.0025在下面有說明...
    Kx = Px / (Px + Rx);                      //計算卡爾曼增益
    agx = agx + Kx * (aax - agx);             //陀螺儀角度與加速度計速度疊加
    Px = (1 - Kx) * Px;                       //更新p值

    Py = Py + 0.0025;
    Ky = Py / (Py + Ry);
    agy = agy + Ky * (aay - agy); 
    Py = (1 - Ky) * Py;
  
    Pz = Pz + 0.0025;
    Kz = Pz / (Pz + Rz);
    agz = agz + Kz * (aaz - agz); 
    Pz = (1 - Kz) * Pz;

    /* kalman end */

    Serial.print(agx);Serial.print(",");
    Serial.print(agy);Serial.print(",");
    Serial.print(agz);Serial.println();
    
}
```
