#!/usr/bin/gnuplot

reset

#set terminal pngcairo font "arial,10" size 600,400
#set terminal pngcairo size 650,500 enhanced dashed font 'Verdana,12'
set terminal pngcairo size 950,500 enhanced dashed font 'Verdana,12'

set output 'barchart.png'

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

set style data histogram
set style histogram cluster gap 2
set style fill solid # border -1
set boxwidth 0.9

set key top left

set yrange[0:1200]

#set grid ytics
set xtics rotate by -55
#set auto x

#set title "satan"

# Heur
#plot \
     #"barplot" using 4:xtic(1) title "HC"   ls 1,\
            #"" using 7         title "SA"   ls 2,\
            #"" using 10        title "ACO"  ls 3,\
            #"" using 13        title "BT"   ls 4,\
            #"" using 16        title "ALL"  ls 5,\
            #"" using 18        title "EXCT" ls 6,\
# Total
plot \
     "barplot" using 3:xtic(1) title "HC"   ls 1,\
            "" using 6         title "SA"   ls 2,\
            "" using 9         title "ACO"  ls 3,\
            "" using 12        title "BT"   ls 4,\
            "" using 15        title "ALL"  ls 5,\
            "" using 18        title "EXCT" ls 6,\

# Time
#plot \
     #"barplot" using 2:xtic(1) title "HC"   ls 1,\
            #"" using 5         title "SA"   ls 2,\
            #"" using 8         title "ACO"  ls 3,\
            #"" using 11        title "BT"   ls 4,\
            #"" using 14        title "ALL"  ls 5,\
            #"" using 17        title "EXCT" ls 6,\
