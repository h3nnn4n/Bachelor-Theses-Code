#!/usr/bin/gnuplot
#
# Colored tics with the epslatex terminal
#
# AUTHOR: Hagen Wierstorf

reset

# epslatex
#set terminal epslatex size 10.4cm,6.35cm color colortext standalone 'phv,9' \
#header '\definecolor{t}{rgb}{0.5,0.5,0.5}'
#set output 'epslatex_correct.tex'
# wxt
#set terminal wxt size 410,250 enhanced font 'Verdana,9' persist
# png
#set terminal pngcairo size 800,600 enhanced font 'Verdana,9'
set terminal pngcairo size 700,300 enhanced dashed font 'Verdana,10'
set output 'nice_web_plot.png'
# svg
#set terminal svg size 410,250 fname 'Verdana, Helvetica, Arial, sans-serif' fsize '9' rounded dashed
#set output 'nice_web_plot.svg'

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set border 3 front ls 11
set tics nomirror
# define grid
set style line 12 lc rgb'#808080' lt 0 lw 1
set grid back ls 12

show style lines

# color definitions
set style line 1 lc rgb '#000000' pt 1 ps 1.5 lw 1 dashtype 1 pi 30# --- red
set style line 2 lc rgb '#000000' pt 2 ps 1.5 lw 1 dashtype 1 pi 30# --- green
set style line 3 lc rgb '#000000' pt 3 ps 1.5 lw 1 dashtype '..'# --- green
set style line 4 lc rgb '#000000' pt 4 ps 1.5 lw 1 dashtype 2 # --- green
set style line 5 lc rgb '#000000' pt 8 ps 1.5 lw 1 dashtype 5 # --- green
set style line 6 lc rgb '#000000' pt 6 ps 1.5 lw 1 dashtype 4 # --- green

set key top right

set key samplen 4

#set format '$'
set xlabel 'Time (s)'
set ylabel 'Objective Function'

#set xrange [0:1]
set yrange [0:1300000]
set xtics rotate by -55
#set format y "%.0s*10^%T"
set format y "%.0tx10^%1T"

plot 'sa/sa_prog'          u (column(0)*1.201):($1) t 'SA'      w lp ls 1, \
     'tabu/tabu_prog'      u (column(0)*0.107):($1) t 'Tabu'    w l ls 2, \
     'aco/aco_prog'        u (column(0)*2.422):($1) t 'ACO'     w l ls 4, \
     'hc/hc_prog'          u (column(0)*0.301):($1) t 'HC'      w l ls 5, \
     'exact/exact_prog'    u (column(0)*2.523):($1) t 'Exact'   w l ls 3, \
     'all/all_prog'        u (column(0)*0.159):($1) t 'Chained' w l ls 6
