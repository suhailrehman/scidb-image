/*
 * patch_avg_mpi.c
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
#include "mpi.h"
#include <cstdlib>
#include "../util/CImg.h"

#include "../util/utils.h"


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

	printf("Reading directory: %s\n",argv[1]);

	start_time = mpi_sync_time();

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

	//Generate random list of weights and copy to all processes:
	float weight_sum=0.0;
	float *weights = (float*) malloc(sizeof(float)*files.size());

	int patch_width = 100;
	int patch_height = 100;
	int patch_x_start = 32;
	int patch_y_start = 32;



	//Image Canvas Properties
	int width;
	int height;
	int channels;

	if(processor_id == master)	//Master Process
	{
		//Generate random list of weights
		rand_image_weights(files.size(),time(NULL),weight_sum, weights);

		//Open first file to get image properties
		CImg<unsigned char> src(files[0].c_str());
		width = src.width();
		height = src.height();
		channels = src.spectrum();
	}

	//Broadcast Weight Array
	MPI::COMM_WORLD.Bcast(weights, files.size(),MPI::FLOAT,master);

	//Broadcast Canvas Properties
	MPI::COMM_WORLD.Bcast(&width, sizeof(int),MPI::INT,master);
	MPI::COMM_WORLD.Bcast(&height, sizeof(int),MPI::INT,master);
	MPI::COMM_WORLD.Bcast(&channels, sizeof(int),MPI::INT,master);

	setup_time = mpi_elapsed_time(start_time);

	/* Debuging that the weights are properly distributed here:
	for (int i = 0; i < files.size(); i++)
	{
		printf("Processor %d, weight[%d]=%.2f\n", processor_id,i, weights[i]);
	}
	*/

	/*Every process get its own canvas to store the image processing
	 *partial result
	 */

	start_time = mpi_sync_time();

	CImg<double> patch(patch_width,patch_height,1,channels,false);

	//Walk through file vector in strided fashion to perform image processing
	for (int i = processor_id; i < files.size(); i += num_processors)
	{
		/*
		printf("Processor %d assigned File: %s\n"
				,processor_id,files[i].c_str());
		*/
		printf("Processing File %d\r",i);
		CImg<unsigned char> next(files[i].c_str());

		//Update local average image
		#pragma omp parallel for
		cimg_forXYC(patch,x,y,c)
		{
			patch(x,y,c) = patch(x,y,c) +
					(next(x+patch_x_start,y+patch_y_start,c) * weights[i]);
		}

	}

	printf("\n");

	//MPI Barrier to Ensure every processor is done.
	MPI::COMM_WORLD.Barrier();

	mapping_time = mpi_elapsed_time(start_time);

	start_time = mpi_sync_time();

	//MPI Requires pre-defined struct sizes and information so that it
	//can handle the struct buffer info - I'm going to send the pixel buffer as
	//a <T> array instead.
	double *image_buffer=NULL;

	if(processor_id == master)
	{
		image_buffer=(double*)malloc(sizeof(double)*patch.size()*num_processors);
	}

	void *image_to_send = patch.data();

	//Master Process Collect Partial Results from everyone

	MPI_Gather(image_to_send,
		patch.size(),
	    MPI::DOUBLE,
	    image_buffer,
		patch.size(),
	    MPI::DOUBLE,
	    master,
		MPI::COMM_WORLD);

	scatter_time = mpi_elapsed_time(start_time);

	start_time = mpi_sync_time();

	//Produce final Image
	if(processor_id == master)
	{

		CImg <double> final_image(patch_width,patch_height,1,channels,false);

		for(int i = 0; i < num_processors; i += 1)
		{
			CImg <double>average_image_n(image_buffer+(i*patch.size()),patch_width,patch_height,1,channels,false);
			//printf("Inside Loop %d\n",i);

			//DEBUG: Custom Output image save
			//char filename[10];
			//sprintf(&filename,"image-%d.jpg",i);
			//average_image_n.save(filename);

			//Update local average image
			//#pragma omp parallel for
			cimg_forXYC(final_image,x,y,c)
			{
				final_image(x,y,c) = final_image(x,y,c) + average_image_n(x,y,c);
			}

		}

		//Divide by weighted sum
		//TODO: Replace with normalized weights

		#pragma omp parallel for
		cimg_forXYC(final_image,x,y,c)
		{
			final_image(x,y,c) = final_image(x,y,c) / weight_sum;
		}

		//TODO: Custom Output image save
		final_image.save("output.jpg");

	}

	reduce_time = mpi_elapsed_time(start_time);

	if(processor_id == master)
	{
		printf("Time: Setup,Mapping,Scatter,Reduce\n");
		printf("Time: %.2f,%.2f,%.2f,%.2f\n "
						,setup_time,mapping_time,scatter_time,reduce_time);
	}

	MPI::Finalize();

}

