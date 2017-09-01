#include <execinfo.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <inc/utils/Quaternion.h>
#include <inc/utils/Vector3d.h>
#include <inc/utils/RotationMatrix.h>
#include <inc/utils/Matrix3x3d.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <inc/HMDDevice.h>
#include <inc/ControllerDevice.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <unistd.h>

#include <inc/AllanVariance.h>

#define PTHREAD_POLLING_QUATERNION
#define LIBGLM_VERSION_09721 // libglm-dev (0.9.7.2-1)
//
#define ALLAN_VARIANCE_CNT 60

using namespace std;

/**********************Global Variable definition***********************/
char title[] = "3D Shapes";

enum device {
    NONE,
    HMD,
    HMD_DK1,
    CONTROLLER
};

enum mode {
    REALTIME,
    PLAYBACK,
    ALLANPILOT,
};

enum {
    IDX_GYROX,
    IDX_GYROY,
    IDX_GYROZ,
    IDX_ACCX,
    IDX_ACCY,
    IDX_ACCZ,
    IDX_MAX,
};

double gEvent[IDX_MAX] = { 0.0 };

device gDevice = HMD;
mode gMode = REALTIME;
char gFilePath[200];
int gPlaybackIdx = 0;
double gScaling = -5.0f;

HmdDevice* gHmdDriver = NULL;
ControllerDevice* gControllerDriver = NULL;
pthread_t tid;
Quaternion Q(0.0f, 0.0f, 0.0f, 0.0f);
static int maxSampleCount = 0;

#ifdef LIBGLM_VERSION_09721
glm::mat4 gMatrix;
#else
glm::core::type::mat4 gMatrix;
#endif

int gODR = 0;
long long sStartTime = 0;

/*Allan Variance*/
bool bAllanVariance = false;
AllanVariance gAVAD;

void* readerThreadFn(void*);
void readFile(Quaternion& q, int frameid);

#ifdef LIBGLM_VERSION_09721
void dumpMatrix(glm::mat4 m);
#else
void dumpMatrix(glm::core::type::mat4 m);
#endif

/* Initialize OpenGL Graphics */
void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f,
                 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,
           GL_NICEST);  // Nice perspective corrections
}

void loadDeviceDriver()
{
    switch (gDevice) {
    case HMD:
        gHmdDriver = new HmdDevice(false);

        if (gHmdDriver != NULL) {
            gHmdDriver->init(gHmdDriver);
#ifdef PTHREAD_POLLING_QUATERNION
            pthread_create(&tid, NULL, readerThreadFn, NULL);
#endif // PTHREAD_POLLING_QUATERNION
        }

        break;

    case HMD_DK1:
        gHmdDriver = new HmdDevice(true);

        if (gHmdDriver != NULL) {
            gHmdDriver->init(gHmdDriver);
#ifdef PTHREAD_POLLING_QUATERNION
            pthread_create(&tid, NULL, readerThreadFn, NULL);
#endif // PTHREAD_POLLING_QUATERNION
        }

        break;

    case CONTROLLER:
        gControllerDriver = new ControllerDevice();
        break;

    default:
        cout << "ERROR! loadDeviceDriver fail" << endl;
        break;
    }
}

void QuatToMatrix(float m[3][3], Quaternion& q)
{
    double sqw = q.w * q.w;
    double sqx = q.x * q.x;
    double sqy = q.y * q.y;
    double sqz = q.z * q.z;

    m[0][0] = (sqx - sqy - sqz + sqw);
    m[1][1] = (-sqx + sqy - sqz + sqw);
    m[2][2] = (-sqx - sqy + sqz + sqw);

    double tmp1 = q.x * q.y;
    double tmp2 = q.z * q.w;
    m[1][0] = 2.0 * (tmp1 + tmp2);
    m[0][1] = 2.0 * (tmp1 - tmp2);

    tmp1 = q.x * q.z;
    tmp2 = q.y * q.w;
    m[2][0] = 2.0 * (tmp1 - tmp2);
    m[0][2] = 2.0 * (tmp1 + tmp2);
    tmp1 = q.y * q.z;
    tmp2 = q.x * q.w;
    m[2][1] = 2.0 * (tmp1 + tmp2);
    m[1][2] = 2.0 * (tmp1 - tmp2);
}

static long long now_ms(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return (long long)(res.tv_sec * 1000 + res.tv_nsec / 1e6);
}

void* readerThreadFn(void* data)
{
    unsigned char answer[64] = {0};
    Quaternion Q(0.0f, 0.0f, 0.0f, 0.0f);
    static int count = 0;
    static int adCount = 0;
    static int adSec = 0;

    while (1) {
        if (sStartTime == 0) {
            sStartTime = now_ms();
        }

        memset(answer, 0, 64);
#ifdef PTHREAD_POLLING_QUATERNION

        if (gHmdDriver != NULL) {
            gHmdDriver->parseUsbPacket(answer, Q, gEvent);
        }

        //cout << Q << endl;
        if (isnan(Q.w)) {
            cout << "Device is not connect, try to reconnect!!\n";
            sleep(5);
            delete gHmdDriver;
            gHmdDriver = NULL;

            if (gDevice == HMD) {
                gHmdDriver = new HmdDevice(false);
            } else if (gDevice == HMD_DK1) {
                gHmdDriver = new HmdDevice(true);
            }

            if (gHmdDriver != NULL) {
                gHmdDriver->init(gHmdDriver);
            }
        }

#endif // PTHREAD_POLLING_QUATERNION

        if (bAllanVariance) {
            gAVAD.appendFrame(gEvent[0], gEvent[1], gEvent[2], gEvent[3], gEvent[4],
                              gEvent[5]);
            adCount++;

            int odr = (gODR != 0) ? gODR : 400;

            if (adCount % (odr) == 0) {
                adSec++;

                if (ALLAN_VARIANCE_CNT - adSec > 0) {
                    cout << "Record for " << ALLAN_VARIANCE_CNT - adSec << " seconds!!\n";
                } else {
                    cout << "Allan Variance plot show\n";
                }
            }

            if (adCount == odr * ALLAN_VARIANCE_CNT) {
                bAllanVariance = false;
                gAVAD.computeAllanVariance();
                cout << "==============Bias Instability====================\n";
                gAVAD.getBiasInstability();
                adCount = 0;
                adSec = 0;
            }
        }

        if (now_ms() - sStartTime > 1000) {
            gODR = count;
            count = 0;
            sStartTime = 0;
        } else {
            count++;
        }
    }
}

#define PI 3.14159265
void getEulerAngles(float angle[])
{
    double roll, pitch, yaw;
    //atan2(2(q0*q1+q2*q3), 1-2(q1*q1+q2*q2))
    roll = atan2(2 * (Q.w * Q.x + Q.y * Q.z), 1 - 2 * (pow(Q.x, 2) + pow(Q.y,
                 2))) * 180.0 / PI;

    //asin(2(q0*q2-q3*q1))
    pitch = asin(2 * (Q.w * Q.y - Q.z * Q.x)) * 180.0 / PI;

    //atan2(2(q0*q3+q1q2), 1-2(q2*q2+q3*q3))
    yaw = atan2(2 * (Q.w * Q.z + Q.x * Q.y), 1 - 2 * (pow(Q.y, 2) + pow(Q.z,
                2))) * 180.0 / PI;

    //cout << roll << " " << pitch << " " << yaw << endl;
    if (angle != NULL) {
        angle[0] = roll;
        angle[1] = pitch;
        angle[2] = yaw;
    }
}

#ifdef LIBGLM_VERSION_09721
void dumpMatrix(glm::mat4 m)
{
#else
void dumpMatrix(glm::core::type::mat4 m)
{
#endif

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cout << m[i][j] << " ";
        }

        cout << endl;
    }

    cout << "------------\n";
}

void drawContent()
{
    glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
    // Top face (y = 1.0f)
    // Define vertices in counter-clockwise (CCW) order with normal pointing out

    glColor3f(0.0f, 1.0f, 0.0f);     // Green
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f,  1.0f);
    glVertex3f(1.0f, 1.0f,  1.0f);

    // Bottom face (y = -1.0f)
    glColor3f(1.0f, 0.5f, 0.0f);     // Orange
    glVertex3f(1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);

    // Front face  (z = 1.0f)
    glColor3f(1.0f, 0.0f, 0.0f);     // Red
    glVertex3f(1.0f,  1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);

    // Back face (z = -1.0f)
    glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(1.0f,  1.0f, -1.0f);

    // Left face (x = -1.0f)
    glColor3f(0.0f, 0.0f, 1.0f);     // Blue
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);

    // Right face (x = 1.0f)
    glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
    glVertex3f(1.0f,  1.0f, -1.0f);
    glVertex3f(1.0f,  1.0f,  1.0f);
    glVertex3f(1.0f, -1.0f,  1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glEnd();  // End of drawing color-cube
}

/* Handle for Idle event. Called back when there is idle. We treat it to read
 * hmd device's quaternion to rotate color cube.
 */
void idle()
{
    static long long start = 0;
    static char ttt[64];
    static int FPS = 0, outFPS;
    float angle[3];
    long long current = now_ms();

    if (start == 0) {
        start = current;
    }

#ifdef LIBGLM_VERSION_09721
    glm::tquat<float> MyQuaternion;
#else
    glm::gtc::quaternion::quat MyQuaternion;
#endif


    // 1. Get quaternion from hmd/controller device
    if (gMode == REALTIME) {
        unsigned char answer[64] = {0};
        memset(answer, 0, 64);

        if (gHmdDriver != NULL) {
            gHmdDriver->parseUsbPacket(answer, Q, NULL);
            //cout << "Hmd: " << Q << endl;
        } else if (gControllerDriver != NULL) {
            gControllerDriver->parseUsbPacket(answer, Q, NULL);
            //cout << "Controller: " << Q << endl;
        }
    } else {
        if (gPlaybackIdx < maxSampleCount) {
            readFile(Q, gPlaybackIdx++);
        }
    }

    // Adjust Quaternion axis
    // Due to original HMD's axis is different than opengl

#if 0

    if (/*(gMode == PLAYBACK && gPlaybackIdx <= maxSampleCount - 1) || */gMode ==
            REALTIME) {
        Vector3d euler(180.0, 0.0, 180.0);
        Quaternion adjust;
        adjust.converEulerTo(euler);
        Matrix3x3d adjustM = adjust.toRotationMatrix();
        Matrix3x3d m = adjustM * Q.toRotationMatrix();
        RotationMatrix rm(m.mInstance[0], m.mInstance[1], m.mInstance[2],
                          m.mInstance[3], m.mInstance[4], m.mInstance[5],
                          m.mInstance[6], m.mInstance[7], m.mInstance[8]);
        Q = rm.toQuat();
    }

#endif

    // 2. Translate to rotation matrix
#ifdef LIBGLM_VERSION_09721
    MyQuaternion = glm::tquat<float>(Q.w, Q.x, Q.y, Q.z);

    glm::tmat4x4<float> RotationMatrix = glm::toMat4(MyQuaternion);
    glm::tmat4x4<float> oRotationMatrix = RotationMatrix * gMatrix;
#else
    MyQuaternion = glm::gtc::quaternion::quat(Q.w, Q.x, Q.y, Q.z);

    glm::core::type::mat4 RotationMatrix = glm::gtx::quaternion::toMat4(
            MyQuaternion);
    glm::core::type::mat4 oRotationMatrix = RotationMatrix * gMatrix;
#endif

    // 3. Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 4. To operate on model-view matrix
    glMatrixMode(GL_MODELVIEW);

    // 5. Reset the model-view matrix
    glLoadIdentity();

    // 6. Move into the screen
    glTranslatef(0.0f, 0.0f, gScaling);

    // 7. Apply rotationMatrix from the HMD's quaternion
    glMultMatrixf(&oRotationMatrix[0][0]);

    // 8. Draw content
    drawContent();

    FPS++;

    if (current - start >= 1000) {
        start = 0;
        //cout << "FPS: " << FPS << endl;
        outFPS = FPS;
        FPS = 0;
    }

    getEulerAngles(angle);
    snprintf(ttt, 1024, "ODR: %d FPS: %d (%.6f, %.6f, %.6f)", gODR, outFPS,
             angle[0], angle[1], angle[2]);
    glutSetWindowTitle(ttt);

    // 7. Swap the front and back frame buffers (double buffering)
    glutSwapBuffers();
}

/* Handler for window-repaint event. Called back when the window first appears and
 *    whenever the window needs to be re-painted. */
void display()
{
    cout << "display" << endl;

    // 1. Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. To operate on model-view matrix
    glMatrixMode(GL_MODELVIEW);

    // 3. Reset the model-view matrix
    glLoadIdentity();

    // 4. Move into the screen
    glTranslatef(0.0f, 0.0f, -5.0f);

    // 5. Render a color-cube consisting of 6 quads with different colors
    drawContent();

    // 7. Swap the front and back frame buffers (double buffering)
    glutSwapBuffers();
}

/* Handler for window re-size event. Called back when the window first appears and
 *    whenever the window is re-sized with its new width and height */
void reshape(GLsizei width,
             GLsizei height)    // GLsizei for non-negative integer
{
    // Compute aspect ratio of the new window
    if (height == 0) {
        height = 1;    // To prevent divide by 0
    }

    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

int getTotalSample()
{
    ifstream ifs(gFilePath);
    int i = 0;
    string line;

    if (ifs.is_open()) {
        while ((getline(ifs, line))) {
            i++;
        }
    }

    return i;
}

void readFile(Quaternion& q, int frameid)
{
    ifstream ifs(gFilePath);
    string line, token;
    int i = 0;
    int idx = 0;
    double quat[4];

    if (ifs.is_open()) {
        while ((getline(ifs, line)) && (i <= frameid)) {
            if (i == frameid) {
                std::istringstream in(line);
                //cout << line << endl;
                idx = 0;

                while (getline(in, token, ',')) {
                    //cout << atof(token.c_str()) << " ";
                    quat[idx++] = atof(token.c_str());
                }

                //cout << "\n";
#if 0
                cout << "(" << quat[3] << ", " << quat[0] << ", " <<
                     quat[1] << ", " << quat[2] << ")\n";
#endif
                q.w = quat[3];
                q.x = quat[0];
                q.y = quat[1];
                q.z = quat[2];
                break;
            } else {
                i++;
            }
        }

        ifs.close();
    }
}

void dumpRadius(const Vector3d& radius)
{
    cout << "(" << radius.getVal(0) * 180 / M_PI << ", " <<
         radius.getVal(1) * 180 / M_PI << ", " << radius.getVal(2) * 180 / M_PI << ")\n";
}

void interceptKeyCallback(unsigned char key, int x, int y)
{
    switch (key) {
    case 'a': {
            cout << "Start to record raw data for Allan variance\n";
            gAVAD.init();
            bAllanVariance = true;
        }
        break;

    case 'c': {
            cout << "Recenter\n";
            dumpMatrix(gMatrix);
#if 1
#ifdef LIBGLM_VERSION_09721
            glm::tquat<float> MyQuaternion;
#else
            glm::gtc::quaternion::quat MyQuaternion;
#endif

#ifdef LIBGLM_VERSION_09721
            MyQuaternion = glm::tquat<float>(Q.w, Q.x, Q.y, Q.z);

            glm::tmat4x4<float> RotationMatrix = glm::toMat4(MyQuaternion);
#else
            MyQuaternion = glm::gtc::quaternion::quat(Q.w, Q.x, Q.y, Q.z);

            glm::core::type::mat4 RotationMatrix = glm::gtx::quaternion::toMat4(
                    MyQuaternion);
#endif
            dumpMatrix(RotationMatrix);

#ifdef LIBGLM_VERSION_09721
            gMatrix = glm::inverse(RotationMatrix);
#else
            gMatrix = glm::core::function::matrix::inverse(RotationMatrix);
#endif

            dumpMatrix(gMatrix * RotationMatrix);
#else
            double yaw = atan2(2 * (Q.y * Q.z + Q.w * Q.x),
                               Q.w * Q.w - Q.x * Q.x - Q.y * Q.y + Q.z * Q.z);
            cout << "yaw: " << (yaw * 180.0) / M_PI << endl;
            Quaternion q;
            Vector3d angle(0.0, yaw * 180.0 / M_PI, 0.0);
            q.converEulerTo(angle);
            cout << q << endl;
            glm::gtc::quaternion::quat MyQuaternion;
            MyQuaternion = glm::gtc::quaternion::quat(q.w, q.x, q.y, q.z);
            glm::core::type::mat4 RotationMatrix = glm::gtx::quaternion::toMat4(
                    MyQuaternion);

#ifdef LIBGLM_VERSION_09721
            gMatrix = glm::inverse(RotationMatrix);
#else
            gMatrix = glm::core::function::matrix::inverse(RotationMatrix);
#endif

            dumpMatrix(gMatrix * RotationMatrix);
#endif

        }
        break;

    case 'q': {
            exit(0);
        }
        break;

    case 'r': {
            gPlaybackIdx = 0;
        }
        break;

    case '+' : {
            gScaling += 0.05f;
        }
        break;

    case '-' : {
            gScaling -= 0.05f;
        }
        break;

    default:
        cout << "Receive: " << key << endl;
        break;
    }
}

void interceptSpecialKeyCallback(int keyCode, int x, int y)
{
    switch (keyCode) {
    case GLUT_KEY_HOME:
        gScaling = -5.0f;
        break;

    default:
        cout << "Receive: " << keyCode << endl;
        break;
    }
}

void printConfiguration()
{
    cout << "--------------- Configuration ------------------\n";
    cout << "\tDevice: ";

    switch (gDevice) {
    case HMD:
        cout << "HMD ";
        break;

    case CONTROLLER:
        cout << "CONTROLLER ";
        break;

    default:
        cout << "UNKNOWN ";
        break;
    }

    cout << endl;
    cout << "\tMode: ";

    switch (gMode) {
    case PLAYBACK:
        cout << "PLAYBACK[" << gFilePath << "]";
        break;

    case ALLANPILOT:
        cout << "AllanPilot[" << gFilePath << "]";
        break;

    default:
        cout << "REALTIME";
        break;
    }

    cout << endl;
    cout << "--------------------------------------------------\n";
}

void printUsage()
{
    cout << "Usage: sudo ./Cube [OPTION]... [DEVICE_TYPE]" << endl;
    cout << "Option list:\n";
    cout << "\t-f\t\t\tChange the running mode to playback, default is realtime\n";
    cout << "\tCMD: -f [FILE PATH]\n";
    cout << endl;
    cout << "Device type list:\n";
    cout << "\t[HMD] for read the pose data from HMD over usb cable\n";
    cout << "\t[CONTROLLER] for read the pose data from CONTROLLER over BLE\n";
}

bool parseCmd(int argc, char** cmd)
{
    bool ret = true;

    for (int i = 1; i < argc; i++) {
        cout << cmd[i] << endl;

        if (strncmp(cmd[i], "HMD", 3) == 0) {
            gDevice = HMD;
        } else if (strncmp(cmd[i], "DK1", 3) == 0) {
            cout << "dk1" << endl;
            gDevice = HMD_DK1;
        } else if (strncmp(cmd[i], "CONTROLLER", 10) == 0) {
            gDevice = CONTROLLER;
        } else if (strncmp(cmd[i], "-f", 2) == 0) {
            gMode = PLAYBACK;

            if (i + 1 == argc) {
                cout << "Invalid parameter with -f\n";
                ret = false;
            } else {
                ifstream f(cmd[++i]);

                if (f.good()) {
                    strcpy(gFilePath, cmd[i]);
                } else {
                    cout << "Playback file " << cmd[i] << " not found!!\n";
                    ret = false;
                }
            }
        } else if (strncmp(cmd[i], "-ad", 3) == 0) {
            gMode = ALLANPILOT;

            if (i + 1 == argc) {
                cout << "Invalid parameter with -f\n";
                ret = false;
            } else {
                ifstream f(cmd[++i]);

                if (f.good()) {
                    strcpy(gFilePath, cmd[i]);
                    gAVAD.init();
                    gAVAD.computeAllanVarianceFromFile(cmd[i]);
                } else {
                    cout << "Sensor raw data " << cmd[i] << " not found!!\n";
                    ret = false;
                }
            }
        }
    }

    return ret;
}

void handler(int sig)
{
    void* array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);

    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv)
{
    signal(SIGSEGV, handler);   // install our handler

    if (!parseCmd(argc, argv)) {
        printUsage();
        return 0;
    }

    printConfiguration();

    if (gMode == ALLANPILOT) {
        return 0;
    }

    if (gMode == REALTIME) {
        loadDeviceDriver();
    } else {
        maxSampleCount = getTotalSample();
        cout << "Total sample: " << maxSampleCount << endl;
    }

    glutInit(&argc, argv);            // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
    glutInitWindowSize(1024, 768);   // Set the window's initial width & height
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutCreateWindow(title);          // Create window with the given title
    glutDisplayFunc(
        display);       // Register callback handler for window re-paint event
    glutReshapeFunc(
        reshape);       // Register callback handler for window re-size event
    glutIdleFunc(
        idle);             // Register callback handler for window idle event
    glutKeyboardFunc(
        interceptKeyCallback); //Register callback handler for window key event
    glutSpecialFunc(
        interceptSpecialKeyCallback);   //Register callback handler for window special key event
    initGL();                       // Our own OpenGL initialization
    glutMainLoop();                 // Enter the infinite event-processing loop
    return 0;
}
