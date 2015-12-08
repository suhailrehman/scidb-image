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
#include "mpi.h"
#include <cstdlib>
#include "../util/CImg.h"

#include "../util/utils.h"


using namespace cimg_library;


/*
 *
 * SSD Computation:
	for each assigned image image[i]:
		for each image in dataset image[j]:
			if(i==j) distance[i][j] = 0;
			else
				distance[i][j] = compute_ssd(image1,image2);

	MPI_Barrier;

	MPI_Gather(distance,,MPI::Double,distance,,)

	compute_knn(distance);

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

	//Walk through file vector in block fashion for each process
	for (int i = BLOCK_LOW(processor_id,num_processors,files.size());
			i <= BLOCK_HIGH(processor_id,num_processors,files.size());
			i ++)
	{
		printf("Processor %d assigned File: %s\n"
				,processor_id,files[i].c_str());
	}
/*
	//MPI Barrier to Ensure every processor is done.
	MPI::COMM_WORLD.Barrier();

	//MPI Requires pre-defined struct sizes and information so that it
	//can handle the struct buffer info - I'm going to send the pixel buffer as
	//a <T> array instead.
	double *image_buffer=NULL;

	if(processor_id == master)
	{
		image_buffer=(double*)malloc(sizeof(double)*avg.size()*num_processors);
	}

	void *image_to_send = avg.data();

	//Master Process Collect Partial Results from everyone

	MPI_Gather(image_to_send,
		avg.size(),
	    MPI::DOUBLE,
	    image_buffer,
		avg.size(),
	    MPI::DOUBLE,
	    master,
		MPI::COMM_WORLD);


	//Produce final Image
	if(processor_id == master)
	{
		CImg <double> final_image(width,height,1,channels,false);

		for(int i = 0; i < num_processors; i += 1)
		{
			CImg <double>average_image_n(image_buffer+(i*avg.size()),width,height,1,channels,false);
			//printf("Inside Loop %d\n",i);

			//DEBUG: Custom Output image save
			//char filename[10];
			//sprintf(&filename,"image-%d.jpg",i);
			//average_image_n.save(filename);

			//Update local average image
			cimg_forXYC(final_image,x,y,c)
			{
				final_image(x,y,c) = final_image(x,y,c) + average_image_n(x,y,c);
			}

		}

		//Divide by weighted sum
		//TODO: Replace with normalized weights
		cimg_forXYC(final_image,x,y,c)
		{
			final_image(x,y,c) = final_image(x,y,c) / weight_sum;
		}

		//TODO: Custom Output image save
		final_image.save("output.jpg");
	}
*/
	MPI::Finalize();

}











