CFLAGS = -O3 -L/usr/X11R6/lib -lm -lpthread -lX11 -lXext -fopenmp 
MPICC = mpicc.mpich2
MPICPP = mpic++.mpich2
MPIRUN = mpirun.mpich2

NP = -np 2
TEST_DIR = /home/suhailr/Downloads/1080p
NTHREADS = 2


mpitest:
	$(MPICC) $(CFLAGS) -o bin/mpitest src/test/mpi_test.c
	
imgavg:
	$(MPICPP) $(CFLAGS) -o bin/img_avg_mpi src/util/utils.c src/img_avg/img_avg_mpi.cpp 
	
apnnmpi:
	$(MPICPP) $(CFLAGS) -o bin/apnn_mpi src/util/utils.c src/apnn/apnn_mpi.cpp

runimgavg: imgavg
	$(MPIRUN) $(NP) bin/img_avg_mpi $(TEST_DIR) $(NTHREADS)

runapnn: apnnmpi
	$(MPIRUN) $(NP) bin/apnn_mpi $(TEST_DIR)

all: runimgavg #mpitest runtest

clean:
	rm -f mpitest
