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
set terminal pngcairo size 640,480 enhanced font 'Verdana,9'
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

# color definitions
set style line 1 lc rgb '#8b1a0e' pt 1 ps 1.5 lt 1 lw 1 # --- red
set style line 2 lc rgb '#5e9c36' pt 6 ps 1.5 lt 1 lw 1 # --- green

set key top right

#set format '$'
set xlabel 'Tempo (s)'
set ylabel 'Custo'

#set xrange [0:1]
set yrange [0:1300000]

plot 'sa/sa_prog'          u (column(0)*1.201):($1) t 'SA'    w l ls 1, \
     'tabu/tabu_prog'      u (column(0)*0.107):($1) t 'Tabu'  w l ls 2, \
     'aco/aco_prog'        u (column(0)*2.422):($1) t 'ACO'   w l ls 4, \
     'hc/hc_prog'          u (column(0)*0.301):($1) t 'HC'    w l ls 5, \
     'exact/exact_prog'    u (column(0)*2.523):($1) t 'Exact' w l ls 3, \
     'all/all_prog'        u (column(0)*0.159):($1) t 'Todos' w l ls 6
