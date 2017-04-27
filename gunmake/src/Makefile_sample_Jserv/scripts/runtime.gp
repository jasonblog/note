reset

set logscale x 2
set xlabel 'N'
set ylabel 'Time (sec)'
set grid
set style fill solid
set key left top
set title 'Wall-clock time - using clock\_gettime()'
set term png enhanced font 'Verdana,10'
set output 'runtime.png'

plot [1000:1000000][0:] 'result_clock_gettime.csv' using 1:2 smooth csplines lw 2 title 'Original', \
'' using 1:4 smooth csplines lw 2 title 'OpenMP (2 threads)', \
'' using 1:6 smooth csplines lw 2 title 'OpenMP (4 threads)', \
'' using 1:8 smooth csplines lw 2 title 'AVX', \
'' using 1:10 smooth csplines lw 2 title 'AVX + unroll looping'

reset

set logscale x 2
set xlabel 'N'
set ylabel 'Diff'
set grid
set style fill solid
set key left top
set title 'Error rate'
set term png enhanced font 'Verdana,10'
set output 'error.png'

plot [1000:1000000][0:] 'result_clock_gettime.csv' using 1:3 smooth csplines lw 2 title 'Original', \
'' using 1:5 smooth csplines lw 2 title 'OpenMP (2 threads)', \
'' using 1:7 smooth csplines lw 2 title 'OpenMP (4 threads)', \
'' using 1:9 smooth csplines lw 2 title 'AVX', \
'' using 1:11 smooth csplines lw 2 title 'AVX + unroll looping'
