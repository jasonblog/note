[Description]

The tool is developed by OpenGL on linux based desktop.
It will read either MVR HMD through USB or MVR Controller for quaternion and Cube to apply the rotation matrix to modelView for rotation.
The title will show the render FPS and Euler angles.

[Environment Setup]

1.1 Please make sure your environment have OpenGL, GLM

   (1) sudo apt-get install cmake xorg-dev libglu1-mesa-dev libglm-dev libglew1.6-dev

   (2) sudo apt-get install libglfw3-dev
       (Note: If your ubuntu apt could not found libglfw3-dev, please enter "prebuild/glfw-3.0.4" and follow the following instruction to install glfw3)

   (3) sudo make

   (4) Note: If build break at glm...., please enable the definition (#define LIBGLM_VERSION_09721) for new libglm version

1.2 Please make sure your environment have bluetooth, Node.js, noble

   (1) sudo apt-get install bluetooth bluez libbluetooth-dev libudev-dev

   (2) curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash -
       sudo apt-get install -y nodejs

   (3) please enter "/noble-master" and run the instruction "sudo npm install" to install Node.js

1.3 Due to PoseUI upgrade for allan variance functionality, please make sure your environment have boost and gnuplot,

   (1) sudo apt-get install libboost-dev, libboost-iostream-dev, libboost-fileysystem-dev, libboost-system-dev
   (2) sudo apt-get install gnuplot

[Execution]

2.1 Test MVR HMD:

   (1) conenct MVR HMD and ubuntu PC via USB cable

   (2) run "sudo ./Cube" or "sudo ./Cube HMD"

   (3) MVR HMD current have following
       1. TANK
       2. DK1

       default HMD type is TANK, if would like to connect DK1, please using following command,
       "sudo ./Cuble dk1" for dk1 with axis correction

2.2 Test MVR Controller:

   (1) plug BT BLE Dongle into ubuntu PC

   (2) make sure what your controller MAC address is and fill in the fild "controllerId" in the file "/noble-master/getControllerIMU.js"

   (3) enter "noble-master" folder and run "sudo node getControllerIMU.js"

   (4) run "sudo ./Cube CONTROLLER"

2.3 Test with record pose playback
   (1) Prepare the record pose data with format [w, x, y, z] as follows,
       0.006881562, -0.1600309, -0.007437279, 0.98706
       0.007541733, -0.1599386, -0.008109181, 0.9870648
       0.007272154, -0.1599845, -0.007744493, 0.9870623
       0.006600833, -0.1600774, -0.006988992, 0.9870576
       0.007326253, -0.1603531, -0.007795315, 0.9870018
       0.006339435, -0.1604834, -0.007030445, 0.9869931
       0.007978582, -0.1606644, -0.008327665, 0.9869418
       0.007739681, -0.1603418, -0.007861935, 0.9869999
       0.008560603, -0.1602465, -0.008390059, 0.9870042
       . . .
       for more detail, could refer to <TOPDIR>/playback/unity.log
    (2) run "sudo ./Cube -f <path-to-pose-data>", i.e. sudo ./Cube -f ./playback/unity.log
    (3) If no "-f" option, will default to read HMD or controller data.

2.4 Cube UI hot key
    'a'    : AllanVariance analysis (New)
    'c'    : Recenter the cube.
    'r'    : [Playback only] Reset to the first pose.
    'q'    : Quit the program
    '+'    : Scaling up the cube
    '-'    : Scaling down the cube
    'Home' : Reset the sacling to default(-5.0f)

2.5 Allan Variance with gnuplot
    (1) On Cube main window press 'a' to start recording gsensor and gyro raw data for
        offline allan variance analysis

    (2) Please make sure your device under static pose for specified time.
        Default is 60 seconds, or you could modified main.cpp "ALLAN_VARIANCE_CNT"

    (3) Every seconds will downcount about the progress.

    (4) If you would like to use offline file for allan variance, please use
        ./Cube -ad <file-path>

        Note: Sensor data format as follows,
        accel.x, accel.y, accel.z, gyro.x, gyro.y, gyro.z

    (5) On gnuplot window, press q could make gnuplot window exit

    (6) When allan variance plot finish, console will show following
        The last line is bias instability with (accelx, accely, accelz,
        gyrox, gyroy, gyroz)

        Allan Variance plot show
        **********Allan Variance Configuration************
        Sensor Data Type: 6
        N: 3340 maxN: 1024  endLogInc: 4
        t0: 0.0025
        **************************************************
        ==============Bias Instability====================
        0.104326 0.975267 1.19526 0.0114514 0.00849895 0.00829251 

        For the bias instability, we could know that on each axis drift
        over time.
