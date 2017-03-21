#!/usr/bin/gnuplot

reset

#set terminal pngcairo size 1920,1080 enhanced font 'Verdana,9'
set terminal pngcairo size 800,600 enhanced font 'Verdana,9'
set output 'stats.png'

set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
set style line 12 lc rgb '#808080' lt 0 lw 1

set grid back ls 12

set style line 1 lc rgb '#8b1a0e' pt 1 ps 1 lt 1 lw 1 # --- red
set style line 2 lc rgb '#5e9c36' pt 2 ps 1 lt 1 lw 1 # --- green
set style line 3 lc rgb '#65393d' pt 3 ps 1 lt 1 lw 1 # --- brown
set style line 4 lc rgb '#3db7c2' pt 4 ps 1 lt 1 lw 1 # --- blue
set style line 5 lc rgb '#f9c386' pt 5 ps 1 lt 1 lw 1 # --- blue
set style line 6 lc rgb '#98cdc5' pt 6 ps 1 lt 1 lw 1 # --- grey-cyan-thing

#set yrange [0:2]
unset key
f(x) = mean_y
fit f(x) '100_42.log' u (column(0)):2 via mean_y

stddev_y = sqrt(FIT_WSSR / (FIT_NDF + 1 ))

# Plotting the range of standard deviation with a shaded background
set label 1 gprintf("Mean = %g", mean_y) at 2, 13
set label 2 gprintf("Standard deviation = %g", stddev_y) at 6, 14
plot mean_y+stddev_y with filledcurves y1=mean_y lt 1 lc rgb "#bbbbdd", \
     mean_y-stddev_y with filledcurves y1=mean_y lt 1 lc rgb "#bbddbb", \
     mean_y w l lt 3, '100_42.log' u (column(0)):2 w p pt 7 lt 1 ps 1

#plot mean_y w l
