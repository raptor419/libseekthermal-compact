reset
set ytics nomirror
set y2tics auto
plot "ceiling.dat" using 1 with lines title "rcentral", "ceiling.dat" using 2 with lines title "rdevtmp" axes x1y2
