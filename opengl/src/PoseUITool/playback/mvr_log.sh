#!/bin/bash
sudo adb logcat -c; sudo adb logcat -v time | grep 'VRCORE Client' | grep -Hrn '] rotation (w, x, y, z):' | sed 's/(standard input):[0-9]*:[0-9]*-[0-9]* [0-9]*:[0-9]*:[0-9]*.[0-9]* I\/VRCORE Client([0-9]*): \[[0-9]*\] rotation (w, x, y, z): //g'
