CFLAGS = -O3 -L/usr/X11R6/lib -lm -lpthread -lX11 -lXext -fopenmp 
MPICC = mpicc.mpich2
MPICPP = mpic++.mpich
MPIRUN = mpirun.mpich

NP = -np 2

TEST_DIR = /home/suhailr/Downloads/1080p

mpitest:
	$(MPICC) $(CFLAGS) -o mpitest mpi_test.c

imgavg:
	$(MPICPP) $(CFLAGS) -o img_avg_mpi img_avg_mpi.cpp

runtest: imgavg
	$(MPIRUN) $(NP) ./img_avg_mpi $(TEST_DIR)

all: imgavg runtest

clean:
	rm -f mpitest