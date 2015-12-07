CFLAGS = -O3 -L/usr/X11R6/lib -lm -lpthread -lX11 -lXext -fopenmp 
MPICC = mpicc.mpich2
MPICPP = mpic++.mpich
MPIRUN = mpirun.mpich

NP = -np 2

mpitest:
	$(MPICC) $(CFLAGS) -o mpitest mpi_test.c

runtest: mpitest
	$(MPIRUN) $(NP) ./mpitest

all: runtest

clean:
	rm -f mpitest