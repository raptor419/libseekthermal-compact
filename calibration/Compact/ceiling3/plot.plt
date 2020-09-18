reset
set ytics nomirror
set y2tics auto
#set yrange [15620:15650]
#set y2range [4230:4240]
plot "ceiling.dat" using 3 with points title "rcentral", "ceiling.dat" using 4 with points title "rdevtmp" axes x1y2
