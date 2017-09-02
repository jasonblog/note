#include <iostream>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <inc/utils/Matrix3x3d.h>
#include <inc/utils/Quaternion.h>
#include <inc/utils/RotationMatrix.h>
#include <inc/utils/Vector3d.h>

using namespace std;

inline double ang2rad(double x)
{
    return x * M_PI / 180.0f;
}

inline double rad2ang(double x)
{
    return x * 180.0f / M_PI;
}

void proc_Vector3dtst()
{
    cout << "---------------Vector3d test case --------------------";
    Vector3d v1(0.0, 0.9, 1.8), v2(1.0, 2.0, 3.0), v3, v4;
    cout << endl;
    cout << "v1(" << v1.length() << "): " << v1 << endl;
    cout << "v2(" << v2.length() << "): " << v2 << endl;
    v1.normalize();
    v2.normalize();
    cout << "v1(" << v1.length() << "): " << v1 << endl;
    cout << "v2(" << v2.length() << "): " << v2 << endl;

    cout << endl;
    cout << "v3 = v1 + v2\n";
    v3 = v1 + v2;
    cout << "v3(" << v3.length() << "): " << v3 << endl;
    cout << "v1(" << v1.length() << "): " << v1 << endl;
    cout << "v2(" << v2.length() << "): " << v2 << endl;

    cout << endl;
    cout << "v3 = v3 - v1\n";
    v3 = v3 - v1;
    cout << "v3(" << v3.length() << "): " << v3 << endl;
    cout << "v1(" << v1.length() << "): " << v1 << endl;
    cout << "v2(" << v2.length() << "): " << v2 << endl;

    cout << endl;
    cout << "v3 == v2\n";

    if (v3 == v2) {
        cout << "v3 == v2\n";
        cout << "v3(" << v3.length() << "): " << v3 << endl;
        cout << "v1(" << v1.length() << "): " << v1 << endl;
        cout << "v2(" << v2.length() << "): " << v2 << endl;
    } else {
        cout << "v3(" << v3.length() << "): " << v3 << endl;
    }

    cout << endl;
    cout << "v4 = v1 * v2\n";
    cout << "v1(" << v1.length() << "): " << v1 << endl;
    cout << "v2(" << v2.length() << "): " << v2 << endl;
    cout << "v4(" << v4.length() << "): " << v4 << endl;
    v4 = v1 * v2;
    cout << "v1(" << v1.length() << "): " << v1 << endl;
    cout << "v2(" << v2.length() << "): " << v2 << endl;
    cout << "v4(" << v4.length() << "): " << v4 << endl;
}

void proc_Matrix3x3dtst()
{
    cout << "---------------Matrix3x3d test case --------------------\n";
    Matrix3x3d m1, m2(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
    cout << "m1: " << m1 << endl;
    cout << "m2: " << m2 << endl;
    Matrix3x3d m3;
    cout << "m3: " << m3 << endl;
    cout << endl;
    cout << "m3 = m2;\n";
    m3 = m2;
    cout << "m1: " << m1 << endl;
    cout << "m2: " << m2 << endl;
    cout << "m3: " << m3 << endl;

    cout << endl;
    cout << "m1.setIdentity();\n";
    m1.setIdentity();
    cout << "m1: " << m1 << endl;
    cout << "m2: " << m2 << endl;
    cout << "m3: " << m3 << endl;

    cout << endl;
    Matrix3x3d m4 = m1 + m2;
    cout << "m1: " << m1 << endl;
    cout << "m2: " << m2 << endl;
    cout << "m3: " << m3 << endl;
    cout << "m4: " << m4 << endl;

    cout << endl;
    Matrix3x3d m5(6, 1, 1, 4, -2, 5, 2, 8, 7);
    cout << "m5: " << m5 << endl;
    cout << "m5 det = " << m5.determinant() << endl;

    cout << endl;
    Matrix3x3d m6 = m5;
    m6.inverse();
    cout << "m6 inverse: " << m6 << endl;
    Matrix3x3d I = m5 * m6;
    cout << "m6 * m6 inverse = " << I << endl;

    cout << endl;
    m6.transpose();
    cout << "m6(t): " << m6 << endl;
}

void proc_Quaterniontst()
{
    cout << "---------------Quaternion test case --------------------\n";
    Quaternion q1, q2(0.70711, 0.70711, 0.0f, 0.0f);
    cout << "q1: " << q1 << endl;
    cout << "q2: " << q2 << endl;
    RotationMatrix rm = q2.toRotationMatrix();
    cout << "rm: " << rm << endl;
    cout << "rm2q: " << rm.toQuat() << endl;
    Vector3d angle = rm.toEuler();
    cout.precision(6);
    cout << "rm2eu: (" << angle.getVal(0) * 180 / M_PI << ", " << angle.getVal(
             1) * 180 / M_PI << ", " <<
         angle.getVal(2) * 180 / M_PI << ")\n";
    RotationMatrix rmi = rm;
    rmi.inverse();
    cout << "rm(inv): " << rmi << endl;
    angle = rmi.toEuler();
    cout << "rmi2eu: (" << angle.getVal(0) * 180 / M_PI << ", " << angle.getVal(
             1) * 180 / M_PI << ", " <<
         angle.getVal(2) * 180 / M_PI << ")\n";
    RotationMatrix recenter = rm.getRecenterMatrix();
    cout << "rm: " << rm << endl;
    cout << "recenter: " << recenter << endl;

    {
        Vector3d angle(30.0f, 0.0f, 0.0f);
        cout << endl;
        cout << "angle: " << angle << endl;
        Quaternion qfe;
        qfe.converEulerTo(angle);
        cout << "Quaternion from angle: " << qfe << endl;
        Vector3d angle2 = qfe.toEuler();
        cout << "QFE to euler: " << angle2.getVal(0) * 180 / M_PI << ", " <<
             angle2.getVal(1) * 180 / M_PI << ", " << angle2.getVal(2) * 180 / M_PI << ")\n";
    }
}

void generate_angle(int degree)
{
    Quaternion q;
    Vector3d euler(180.0, 90.0, 0);
    Quaternion adjust;
    adjust.converEulerTo(euler);
    Vector3d angle = adjust.toEuler();
    cout << adjust << endl;
    cout << "(" << angle.getVal(0) * 180 / M_PI << ", " <<
         angle.getVal(1) * 180 / M_PI << ", " << angle.getVal(2) * 180 / M_PI << ")\n";
    Matrix3x3d adjustM = adjust.toRotationMatrix();

    for (int i = 0; i < degree; i++) {
        Vector3d roll((double) i, 0.0f, 0.0f);
        Vector3d yaw(0.0f, (double) i, 0.0f);
        Vector3d pitch(0.0f, 0.0f, (double) i);
        q.converEulerTo(yaw);
        //cout << q << endl;
        Matrix3x3d m = adjustM * q.toRotationMatrix();
        RotationMatrix rm(m.mInstance[0], m.mInstance[1], m.mInstance[2],
                          m.mInstance[3], m.mInstance[4], m.mInstance[5],
                          m.mInstance[6], m.mInstance[7], m.mInstance[8]);
        q = rm.toQuat();
        cout << q << endl;

    }
}

void proc_recenter(double x, double y, double z)
{
    Vector3d angle(x, y, z);
    Quaternion q1;
    q1.converEulerTo(angle);
    cout << q1 << endl;
    Vector3d radius = q1.toEuler();
    cout << rad2ang(radius.getVal(0)) << endl;
}

bool parseCmd(int argc, char** cmd)
{
    bool ret = true;

    for (int i = 1; i < argc; i++) {
        //cout << cmd[i] << endl;
        if (!strncmp(cmd[i], "-v", 2)) {
            // Vector3d test case
            proc_Vector3dtst();
        } else if (!strncmp(cmd[i], "-m", 2)) {
            // Matrix3x3d test case
            proc_Matrix3x3dtst();
        } else if (!strncmp(cmd[i], "-q", 2)) {
            // Quaternion test case
            proc_Quaterniontst();
        } else if (!strncmp(cmd[i], "-g", 2)) {
            generate_angle(360);
        } else if (!strncmp(cmd[i], "-r", 2)) {
            double x = atof(cmd[++i]);
            double y = atof(cmd[++i]);
            double z = atof(cmd[++i]);
            proc_recenter(x, y, z);
        }
    }

    return ret;
}

int main(int argc, char* argv[])
{
    parseCmd(argc, argv);
#if 0

    if (argc == 1) {
        cout << "Use default to run all test case\n";
        proc_Vector3dtst();
        proc_Matrix3x3dtst();
        proc_Quaterniontst();
    } else {
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                if (argv[i][1] == 'v') {
                    // Vector3d test case
                    proc_Vector3dtst();
                } else if (argv[i][1] == 'm') {
                    // Matrix3x3d test case
                    proc_Matrix3x3dtst();
                } else if (argv[i][1] == 'q') {
                    // Quaternion test case
                    proc_Quaterniontst();
                } else if (argv[i][1] == 'g') {
                    generate_angle(360);
                }
            }
        }

        cout << endl;
    }

#endif
    return 0;
}
