reset
#f(x) = a+b*x
f(x) = a+b*x+c*x**2+d*x**3+e*x**4
a=1
b=1
c=1
d=1
e=1
FIT_LIMIT = 1e-14
fit f(x) "devtmp_vs_difference-from-reference.csv" using 1:2 via a,b,c,d,e
plot "devtmp_vs_difference-from-reference.csv" using 1:2 with points title "correction", "devtmp_vs_difference-from-reference.csv" using 1:(f($1)) with lines title "fitted function"
