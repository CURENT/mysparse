CC = cc
CF = -O -g

I = -I../repos/SuiteSparse/CXSparse/Include -I../repos/SuiteSparse/SuiteSparse_config

CS = ../repos/SuiteSparse/CXSparse/Lib/libcxsparse.a
CH = acc.c add.c mod.c del.c  print.c jac.c err.c sandbox.c create.c

all: $(CS) test1 test2

test1: $(CS) test1.c Makefile $(CS)
	$(CC) $(CF) $(I) -o test1 test1.c $(CH) $(CS) -lm

test2: $(CS) test2.c Makefile $(CS)
	$(CC) $(CF) $(I) -o test2 test2.c $(CH) $(CS) -lm

.PHONY: test1 test2

clean:
	- $(RM) *.o main
