/*
 * utils.c
 *
 *  Created on: Dec 7, 2015
 *      Author: suhailr
 */

#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <dirent.h>
#include <string.h>
#include <values.h>
#include "mpi.h"



/*
 * Read and return a vector of filenames from a specified directory
 */
std::vector<std::string> read_files(char *directory)
{
 	DIR *dir;
    struct dirent *dp;


	std::vector<std::string> file_list;

    dir = opendir(directory);
    std::string directory_string(directory);

    while ((dp=readdir(dir)) != NULL) {

        //printf("debug: %s\n", dp->d_name);

        if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") || dp->d_type == DT_DIR) //Skip subdirectories
        {
            // do nothing
        }
        else
        {
        	std::string file_name(dp->d_name);
            std::string full_path =  directory_string + "/" + file_name;
            file_list.push_back(full_path);
        }
    }

    closedir(dir);
    return file_list;
}

/*
*	Generate random image weight array of floats [0,1]
*	of length n, using a random seed
*/
void *rand_image_weights(int n, int seed, float &sum, float *weights)
{
	srand(seed);

	sum=0;

	for(int i=0;i<n;i++)
	{
		weights[i]= rand() % 10;
		//printf("Weight of Image %d is %.2f\n",i, weights[i]);
		sum+=weights[i];
	}


}

/* MPI Start Time Function */
double mpi_sync_time()
{
	MPI_Barrier(MPI_COMM_WORLD);
	return MPI_Wtime();
}

/* MPI Elapsed Time Function */
double mpi_elapsed_time(double start_time)
{
	MPI_Barrier(MPI_COMM_WORLD);
	return MPI_Wtime() - start_time;
}

/*
 * MergeSort Code
 * Code from: https://github.com/Paradigm4/knn/
 */

typedef struct
{
  double v;			// value
  size_t i;			// index
} value;

int
mergesort (double *d, size_t n, size_t * index)
{
  value t;
  size_t j;
  value *tmp = (value *) malloc (n * sizeof (value));
  if (!tmp)
    return -1;
  value *a = (value *) malloc (n * sizeof (value));
  if (!a)
    {
      free (tmp);
      return -1;
    }
  for (j = 0; j < n; ++j)
    {
      a[j].v = d[j];
      a[j].i = j;
    }
  for (j = 1; j < n; j += 2)
    {
      if (a[j].v < a[j - 1].v)
	{
	  t = a[j];
	  a[j] = a[j - 1];
	  a[j - 1] = t;
	}
    }
  int s = 0;
  size_t p;
  for (p = 2; p != 0 && p < n; p <<= 1, s = !s)
    {
      value *z = tmp;
      size_t i;
      for (i = 0; i < n; i += p << 1)
	{
	  value *x = a + i;
	  value *y = x + p;
	  size_t xn = p < n - i ? p : n - i;
	  size_t yn = (p << 1) < n - i ? p : p < n - i ? n - p - i : 0;
	  if (xn > 0 && yn > 0 && (*y).v < x[xn - 1].v)
	    {
	      for (;;)
		{
		  if ((*y).v < (*x).v)
		    {
		      *z++ = *y++;
		      if (--yn == 0)
			break;
		    }
		  else
		    {
		      *z++ = *x++;
		      if (--xn == 0)
			break;
		    }
		}
	    }
	  while (xn > 0)
	    {
	      *z++ = *x++;
	      --xn;
	    }
	  while (yn > 0)
	    {
	      *z++ = *y++;
	      --yn;
	    }
	}
      z = a;
      a = tmp;
      tmp = z;
    }
  if (s)
    {
      value *x, *y;
      size_t k;
      x = tmp;
      y = a;
      for (k = 0; k < n; ++k)
	*x++ = *y++;
    }
  free (tmp);
  for (j = 0; j < n; ++j)
    {
      d[j] = a[j].v;
      if (index)
	index[j] = a[j].i;
    }
  free (a);
  return 0;
}

/*
 * RowSort Code
 * Code from: https://github.com/Paradigm4/knn/
 *
 * rowsort
 * Sort values in each row of a matrix, returning their rank orders.
 * I/O:
 * ans:   A pointer to storage space for the sorted output allocated by caller.
 * data:  A pointer to a block of doubles consisting of one or more full rows
 *        of a matrix. Same size as ans.
 * size:  The total number of data values.
 * nrows: The number of rows of data, must divide size.
 *
 * Returns 0 on success, -1 on error.
 *
 * Example input:
 *
 * 0.0  3.1  2.5  4.5  1.2
 * 4.4  0.0  0.4  0.5  1.0
 * 1.1  1.2  0.0  1.3  1.4
 *
 * Corresponding output:
 *
 * 1.0  4.0  3.0  5.0  2.0
 * 5.0  1.0  2.0  3.0  4.0
 * 2.0  3.0  1.0  4.0  5.0
 */

int
rowsort(double *ans, double *data, size_t size, size_t nrows)
{
  size_t j, l;
  int k = 0;
  if(size % nrows !=0) return -1; // problem not correctly shaped
  size_t dim = size/nrows;
  if(dim>MAXDOUBLE) return -1;  // problem too large
  size_t *i = (size_t *)malloc(dim * sizeof(size_t));
  if(!i) return -1;
  memcpy(ans, data, size*sizeof(double));
  for(j=0;j<nrows;++j)
  {
    k = mergesort (&ans[j*dim], dim, i);
    if(k<0) break; // An error!
// Return just the order results
    for(l=0;l<dim;++l) ans[j*dim + i[l]] = l;
  }
  free(i);
  return 0;
}

