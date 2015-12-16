#!/bin/sh

if [ $# != 3 ]; then
    echo 'Usage: addr2line.sh executefile addressfile functionfile'
    exit
fi;

cat $2 | while read line
do
     if [ "$line" = 'Enter' ]; then
             read line1
             read line2
#           echo $line >> $3
             addr2line -e $1 -f $line1 -s >> $3
             echo "--->" >> $3
             addr2line -e $1 -f $line2 -s | sed 's/^/    /' >> $3
             echo >> $3
     elif [ "$line" = 'Exit' ]; then
             read line1
             read line2
             addr2line -e $1 -f $line2 -s | sed 's/^/    /' >> $3
             echo "<---" >> $3
             addr2line -e $1 -f $line1 -s >> $3
#           echo $line >> $3
             echo >> $3
     fi;
done
