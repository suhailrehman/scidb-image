CFLAGS = -O3 -L/usr/X11R6/lib -lm -lpthread -lX11 -lXext -fopenmp 
MPICC = mpicc.mpich2
MPICPP = mpic++.mpich
MPIRUN = mpirun.mpich

NP = -np 2
TEST_DIR = /home/suhailr/Downloads/1080p


mpitest:
	$(MPICC) $(CFLAGS) -o mpitest test/mpi_test.c
	
imgavg:
	$(MPICPP) $(CFLAGS) -o img_avg_mpi util/utils.c img_avg/img_avg_mpi.cpp 

runtest: imgavg
	$(MPIRUN) $(NP) ./img_avg_mpi $(TEST_DIR)

all: runtest

clean:
	rm -f mpitest
