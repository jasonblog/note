reset

set logscale x 2
set xlabel 'N'
set ylabel 'Diff'
set grid
set style fill solid
set key left top
set title 'Error rate'
set term png enhanced font 'Verdana,10'
set output 'methods_error_rate.png'

plot [4096:65535][0:] 'methods_error_rate.csv' using 1:2 smooth csplines lw 2 title 'Baseline', \
'' using 1:3 smooth csplines lw 2 title 'Leibniz', \
'' using 1:4 smooth csplines lw 2 title 'Euler'
