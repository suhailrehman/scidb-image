/*
 * apnn_mpi.c
 *
 *  Created on: Dec 3, 2015
 *      Author: suhailr
 */

#define cimg_display 0	//Disable CImg's X window functions

#include <stdio.h>
#include <time.h>
#include <string>
#include <vector>
#include <omp.h>
#include <math.h>
#include "mpi.h"
#include <cstdlib>
#include "../util/CImg.h"

#include "../util/utils.h"

double compute_ssd(const char *imagefile1 ,const char* imagefile2);

using namespace cimg_library;


/*
 *
 * APNN Computation:
	for each assigned image image[i]:
		for each image in dataset image[j]:
			if(i==j) distance[i][j] = 0;
			else
				distance[i][j] = compute_ssd(image1,image2);

	compute_knn(rank_matrix,distance);

	MPI_Gather(distance_matrix,,MPI::Double,distance,,)


*/

int main (int argc, char* argv[])
{

	//TODO: getopt parsing

	int master = 0;

	MPI::Init (argc, argv);      /* starts MPI */
	int num_processors = MPI::COMM_WORLD.Get_size();
	int processor_id = MPI::COMM_WORLD.Get_rank();

	//printf("Reading directory: %s\n",argv[1]);
	std::vector<std::string> files = read_files(argv[1]);

	if(argc==3) //If thread count is specified
	{
		printf("Setting Thread Count to %d\n",atoi(argv[2]));
		omp_set_num_threads(atoi(argv[2]));
	}

	/* Debuging vector access here
	for (int i = 0; i < files.size(); i++)
	{
		printf("File Name: %s\n", files[i].c_str());
	}
	*/


	//Image Canvas Properties
	int width;
	int height;
	int channels;

	if(processor_id == master)	//Master Process
	{

		//Open first file to get image properties
		CImg<unsigned char> src(files[0].c_str());
		width = src.width();
		height = src.height();
		channels = src.spectrum();
	}

	//Broadcast Canvas Properties
	MPI::COMM_WORLD.Bcast(&width, sizeof(int),MPI::INT,master);
	MPI::COMM_WORLD.Bcast(&height, sizeof(int),MPI::INT,master);
	MPI::COMM_WORLD.Bcast(&channels, sizeof(int),MPI::INT,master);


	/*Every process gets its own canvas to store the image processing
	 *partial result
	 */
	CImg<double> avg (width,height,1,channels,false);

	/*
	 * Debug
	printf("Process: %d, LOW: %d, HIGH: %d\n",processor_id,BLOCK_LOW(processor_id,num_processors,files.size()), BLOCK_HIGH(processor_id,num_processors,files.size()));
	 */

	//Create a matrix of sum-squared distances of each image
	double *ssdmatrix =
			(double*) malloc(files.size()*files.size()*sizeof(double*));


	//Walk through file vector in block fashion for each process
	for (int i = BLOCK_LOW(processor_id,num_processors,files.size());
			i <= BLOCK_HIGH(processor_id,num_processors,files.size());
			i ++)
	{
		printf("Processor %d assigned File: %s\n"
				,processor_id,files[i].c_str());

		for(int j=0;j<files.size();j++)
		{
			if(i==j) ssdmatrix[i* files.size() + j] = 0;
			else
			{
				ssdmatrix[i* files.size() + j]
						  = compute_ssd(files[i].c_str(),files[j].c_str());
			}

		}
	}


	//Create a matrix of sum-squared distances of each image
	double *rankmatrix	=
			(double*) malloc(files.size()*files.size()*sizeof(double*));

	/* SSD Matrix computed, now rank the values */
	int start_value = BLOCK_LOW(processor_id,num_processors,files.size());

	double * block_ssd_buffer = ssdmatrix + (start_value * files.size());
	double * rank_matrix_buffer = rankmatrix + (start_value *files.size());


	//DEBUG:
	//printf("start_value: %d\n",start_value);
	//printf("size: %d\n",BLOCK_SIZE(processor_id,num_processors,files.size())*files.size());
	//printf("num_rows: %d\n",BLOCK_SIZE(processor_id,num_processors,files.size()));

	int retval = rowsort(rank_matrix_buffer,
			block_ssd_buffer,
			BLOCK_SIZE(processor_id,num_processors,files.size())*files.size(),
			BLOCK_SIZE(processor_id,num_processors,files.size())
			);

	if(retval!=0) printf("Error computing partial weights\n");

	int *counts, *displacements;

	if(processor_id == master){
		counts=(int*)malloc(num_processors*sizeof(int));
		displacements=(int*)malloc(num_processors*sizeof(int));

		for(int i=0;i<num_processors;i++){
			displacements[i] =
					BLOCK_LOW(i,num_processors,files.size())*files.size();

			counts[i] = BLOCK_SIZE(i,num_processors,files.size())*files.size();
		}


	}

	//Collect Rank Matrix Results:
	MPI_Gatherv(rank_matrix_buffer,
			BLOCK_SIZE(processor_id,num_processors,files.size())*files.size(),
			MPI::DOUBLE,
			rankmatrix,
			counts,
			displacements,
			MPI::DOUBLE,
			master,
			MPI::COMM_WORLD);


	/* Print Rank matrix */
	if(processor_id == master)
	{
		for(int i=0;i<files.size();i++)
		{
			for(int j=0;j<files.size();j++)
			{
				printf("%.0f ",rankmatrix[i*files.size()+j]);
			}

			printf("\n");
		}
	}


	MPI::Finalize();

}


/* Given image file names 1 and 2, open, iterate through the
 * pixel buffers and return the sum of squared distances of each pixel value
 */
double compute_ssd(const char *imagefile1 ,const char* imagefile2)
{

	double sum = 0.0;

	CImg<unsigned char> image1(imagefile1);
	CImg<unsigned char> image2(imagefile2);

	if(image1.width() != image2.width() ||
		image1.height()  != image2.height()  ||
		image1.spectrum() != image1.spectrum() ||
		image1.depth() != image1.depth())
	{
		return sum; //Return 0 if the images are of different dimensions
	}

	cimg_forXYC(image1,x,y,c)
	{
		sum += pow(image1(x,y,c) - image2(x,y,c),2);
	}


	return sum;
}









