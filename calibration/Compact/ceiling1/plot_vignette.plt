reset
set ytics nomirror
set y2tics auto
plot "devtemp_vs_vignette.csv" using 1 with lines title "devtemp", "devtemp_vs_vignette.csv" using 2 with lines title "rborder-rcentral" axes x1y2
