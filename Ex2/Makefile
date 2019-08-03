output: mean.o libmath.a 

mean.o: mean.c
	gcc -O -c mean.c

libmath.a: mean.o #let's link library files into a static library
	ar rcs libmath.a mean.o


