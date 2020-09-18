reset
f(x) = a+b*x
a=1
b=1
FIT_LIMIT = 1e-14
fit f(x) "devtmp_vs_difference-from-reference.csv" using 1:2 via a,b
plot "devtmp_vs_difference-from-reference.csv" using 1:2 with points title "correction", "devtmp_vs_difference-from-reference.csv" using 1:(f($1)) with lines title "fitted function"
