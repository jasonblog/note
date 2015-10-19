#!/bin/sh

pause() {
    read -n 1 -p "$*" INP
    if [ "$INP" != '' ] ; then
        echo -ne '\b \n'
    fi
}


mount -t debugfs none /sys/kernel/debug
echo 0 > /sys/kernel/debug/tracing/tracing_on
echo  > /sys/kernel/debug/tracing/trace
pause 'press any key to start capturing...'

echo function_graph > /sys/kernel/debug/tracing/current_tracer
echo funcgraph-proc > /sys/kernel/debug/tracing/trace_options
echo 1 > /sys/kernel/debug/tracing/tracing_on

echo "Start recordng ftrace data"
pause "Press any key to stop..."
echo "Recording stopped..."
echo 0 > /sys/kernel/debug/tracing/tracing_on
