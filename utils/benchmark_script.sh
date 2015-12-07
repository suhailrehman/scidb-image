#SSH to Master

git pull scidb-image
cd mpi
make
cd ..
ls

for num_iterations
	time mpirun.mpich -np $NUM_PROCS bin/img_avg_mpi $IMAGE_DIR
