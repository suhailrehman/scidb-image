/*
 * convolution_mpi.cpp
 *
 *  Created on: Dec 10, 2015
 *      Author: suhailr
 */

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
#include <libgen.h>
#include "../util/CImg.h"

#include "../util/utils.h"
#define KERNEL_WIDTH 3
#define KERNEL_HEIGHT 3

int compute_convolution(const char *imagefile1, const char *outputdirectory);

using namespace cimg_library;


int main (int argc, char* argv[])
{

	//TODO: getopt parsing

	int master = 0;

	MPI::Init (argc, argv);      /* starts MPI */
	int num_processors = MPI::COMM_WORLD.Get_size();
	int processor_id = MPI::COMM_WORLD.Get_rank();

	//Initialize timing variables:
	double start_time, setup_time, mapping_time, scatter_time, reduce_time, output_time;

	start_time = mpi_sync_time();

	//printf("Reading directory: %s\n",argv[1]);
	std::vector<std::string> files = read_files(argv[1]);

	char *outputdir = argv[2];
	/* Debuging vector access here
	for (int i = 0; i < files.size(); i++)
	{
		printf("File Name: %s\n", files[i].c_str());
	}
	*/


	setup_time = mpi_elapsed_time(start_time);

	start_time = mpi_sync_time();


	//Walk through file vector in block fashion for each process
	for (int i = BLOCK_LOW(processor_id,num_processors,files.size());
			i <= BLOCK_HIGH(processor_id,num_processors,files.size());
			i ++)
	{
		printf("Processor %d assigned File: %s\n"
				,processor_id,files[i].c_str());

		if(compute_convolution(files[i].c_str(),outputdir))
		{
			printf("Error: %d, %s\n",processor_id,files[i].c_str());

		}
	}


	mapping_time = mpi_elapsed_time(start_time);

	if(processor_id == master)
		{
			printf("Time: Setup,Mapping\n");
			printf("Time: %.2f,%.2f\n "
							,setup_time,mapping_time);
		}


	MPI::Finalize();

}


/* Given image file name and output directory, apply convolution kernel and
 * write output image
 */
int compute_convolution(const char *imagefile1, const char *outputdirectory)
{

	float kernel[KERNEL_HEIGHT][KERNEL_WIDTH] = {
		   {1,1,1} ,   /*  initializers for row indexed by 0 */
		   {1,1,1} ,   /*  initializers for row indexed by 1 */
		   {1,1,1}   /*  initializers for row indexed by 2 */
		};

	double sum = 0.0;

	CImg<unsigned char> image(imagefile1);

	//Create initial black image
	CImg<double> output (image.width(),image.height(),1,image.depth(),0);

	cimg_forXYC(output,x,y,c)
	{
		float sum= 0.0;

		for(int i = 0; i < KERNEL_HEIGHT; i++)
		{
			for(int j = 0; j < KERNEL_WIDTH; j++)
			{
				if((x-(KERNEL_WIDTH/2)+j) > 0 && (x-(KERNEL_WIDTH/2)+j) < image.width() && (y-(KERNEL_HEIGHT/2)+i) > 0 && (y-(KERNEL_HEIGHT/2)+i) < image.height())
				{

					int xcoord = x-(KERNEL_WIDTH/2)+j;
					int ycoord = y-(KERNEL_HEIGHT/2)+i;
					sum += image(xcoord,ycoord,c)*kernel[i][j];
				}
			}
		}

		output(x,y,c)=sum;

	}

	char *filename = (char*) malloc(512*sizeof(char));
	strcpy(filename, imagefile1);
	filename = basename(filename);



	char outputpath[512];
	strcpy(outputpath,outputdirectory);

	strcat(outputpath,filename);

	printf("Writing output file %s\n",outputpath);

	output.save(outputpath);

	return 0;
}











