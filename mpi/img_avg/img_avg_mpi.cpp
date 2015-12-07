/*
 * img_avg_mpi.c
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
#include "CImg.h"


using namespace cimg_library;
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

int main (int argc, char* argv[])
{

	//TODO: getopt parsing

	int master = 0;

	MPI::Init (argc, argv);      /* starts MPI */
	int num_processors = MPI::COMM_WORLD.Get_size();
	int processor_id = MPI::COMM_WORLD.Get_rank();

	printf("Reading directory: %s\n",argv[1]);
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

	/* Debuging that the weights are properly distributed here:
	for (int i = 0; i < files.size(); i++)
	{
		printf("Processor %d, weight[%d]=%.2f\n", processor_id,i, weights[i]);
	}
	*/

	/*Every process get its own canvas to store the image processing
	 *partial result
	 */
	CImg<double> avg (width,height,1,channels,false);

	//Walk through file vector in strided fashion to perform image processing
	for (int i = processor_id; i < files.size(); i += num_processors)
	{
		printf("Processor %d assigned File: %s\n"
				,processor_id,files[i].c_str());

		CImg<unsigned char> next(files[i].c_str());

		//Update local average image
		cimg_forXYC(avg,x,y,c)
		{
			avg(x,y,c) = avg(x,y,c) + (next(x,y,c) * weights[i]);
		}

	}

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

	MPI::Finalize();

}



	/*

	printf("Using %d thread(s) to compute average\n", omp_get_max_threads());


	//Generate random list of weights;
	float weight_sum;
	float *weights = rand_image_weights(filecount,time(NULL),weight_sum);

	//Load first image to get dimensions -
	//TODO: replace with canvas properties for the future
	CImg<unsigned char> src(files->filename);
	int width = src.width();
	int height = src.height();
	int channels = src.spectrum();

	//Create initial black image
	CImg<double> avg (width,height,1,3,0);

	int count = 0;
	double start_time, weighted_sum_time=0, scalar_divide_time=0;
	//TODO: Loop over all images in directory
	while(ptr!=NULL)
	{
		//Load next image
		CImg<unsigned char> next(ptr->filename);

		start_time = omp_get_wtime();

		#pragma omp parallel for
		cimg_forXYC(avg,x,y,c)
		{
	   		avg(x,y,c) = avg(x,y,c) + (next(x,y,c) * weights[count]);

		}


		weighted_sum_time += (omp_get_wtime() - start_time);


 		ptr=ptr->next;
		count++;
	}

	start_time = omp_get_wtime();

	#pragma omp parallel for
	cimg_forXYC(avg,x,y,c)
	{
		avg(x,y,c) = avg(x,y,c) / weight_sum;
	}

	scalar_divide_time = (omp_get_wtime() - start_time);

	printf("Weighted Sum Time: %.6f seconds\n",weighted_sum_time);
	printf("Scalar Division Time: %.6f seconds\n",scalar_divide_time);

	//TODO: Custom Output image save
	avg.save("output.jpg");

	*/





