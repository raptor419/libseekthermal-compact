reset
set key center center
set xrange [6520:7565]
set y2tics autofreq tc lt 1
plot "boil-water.dat" using 0:3 with lines title "rcentral", (15634) title "17 °C reference", (19614) title "boiling water surface" 
#, "boil-water.dat" using 0:4 with lines title "rdevtmp" axes x1y2
