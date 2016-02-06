# Preparing Raspberry PI for JTAG Debugging


This tutorial shows how to prepare your Raspberry PI board for debugging with JTAG. Using JTAG will allow debugging the Linux kernel using hardware like Segger J-Link. As Raspberry PI board does not have a connector with a normal JTAG layout, preparing the board for JTAG debugging involves finding the correct pins and wiring them in the correct order. Follow the steps below for a complete walkthrough:



- Determine the revision of your Raspberry PI board. Connect to it using SSH and run the following command:

```c
cat /proc/cpuinfo

```

