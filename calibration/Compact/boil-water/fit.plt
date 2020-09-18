reset
plot "boil-water.dat" every 11::6460::7565 using (0.0201416*$3-24.8-273) with lines title "rcentral", "reference.dat" title "reference"
