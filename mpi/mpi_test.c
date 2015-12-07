/* C Example */
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>



int main (argc, argv)
     int argc;
     char *argv[];
{
  char hostname[1024];
  gethostname(hostname, 1024);
  int rank, size;
  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  printf("Hello world from process %d of %d on %s\n", rank, size, hostname);
  MPI_Finalize();
  return 0;
}
