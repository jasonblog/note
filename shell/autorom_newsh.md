# AutoROM_new.sh

```sh
#! /bin/bash

TOP=`pwd`

ls -F | grep "/" > menu.txt

i=0
while read line
do
    echo "$i)" "$line"
    VARS[$i]="$line"
    i=$(($i+1))
done < ./menu.txt

rm ./menu.txt

echo -n "Choose your Project number : "
read choice

platformROM=`basename ${VARS[$choice]}`
echo $platformROM

cd $platformROM

for file in *.zip
do
    if [[ $(echo $file | grep -i "partial") != "" ]]; then
        PARTIAL_NAME=$file
    elif [[ $(echo $file | grep -i "sign") != "" ]]; then
        SIGN_NAME=$file
    else [[ $(echo $file | grep -i "boot") != "" ]];
        BOOT_NAME=$file
    fi
done

cd $TOP

PARTIAL_PATH="$platformROM""/""$PARTIAL_NAME"
BOOT_PATH="$platformROM""/""$BOOT_NAME"
SIGN_PATH="$platformROM""/""$SIGN_NAME"

echo "###################################################################################################################################"
echo $PARTIAL_PATH
echo $BOOT_PATH
echo $SIGN_PATH
echo "###################################################################################################################################"

adb reboot bootloader

htc_fastboot oem rebootRUU
sleep 8
echo "PARTIAL ROM[+++++++++++++++++]"
htc_fastboot flash zip "$platformROM""/""$PARTIAL_NAME"

while [ $? = 1 ]; do
    htc_fastboot oem rebootRUU
    sleep 8
    echo "PARTIAL ROM[22222222222222222]"
    htc_fastboot flash zip "$platformROM""/""$PARTIAL_NAME"
done
echo "PARTIAL ROM[-----------------]"

htc_fastboot oem rebootRUU
sleep 8
echo "BOOT ROM[+++++++++++++++++++]"
htc_fastboot flash zip "$platformROM""/""$BOOT_NAME"
echo "BOOT ROM[-------------------]"

htc_fastboot oem rebootRUU
sleep 8
echo "SIGN ROM[+++++++++++++++++++]"
htc_fastboot flash zip "$platformROM""/""$SIGN_NAME"

while [ $? = 1 ]; do
    htc_fastboot oem rebootRUU
    sleep 8
    echo "SIGN ROM[22222222222222222]"
    htc_fastboot flash zip "$platformROM"/"$SIGN_NAME"
done
echo "SIGN ROM[-------------------]"

htc_fastboot reboot
```
