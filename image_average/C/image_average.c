#include <stdio.h>
#include "CImg.h"
#include <time.h>
#include <stdlib.h>

#include <omp.h>

 
using namespace cimg_library;

//LinkedList of Files
typedef struct file_list_t{
	char *filename;
	file_list_t* next;
}file_list;


/*
*	Return a linked list containing all the files in the specified
*	directory, count is passed by reference to hold the count of files
*	in the directory.
*/
file_list * read_files(char *directory, int &count)
{
 	DIR *dir;
    struct dirent *dp;
    char *file_name;

	file_list *head = NULL, *current = NULL;

    count=0;

    dir = opendir(directory);
    while ((dp=readdir(dir)) != NULL) {
        
        //printf("debug: %s\n", dp->d_name);
        
        if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") || dp->d_type == DT_DIR) //Skip subdirectories
        {
            // do nothing (straight logic)
        } 
        else
        {
            file_name = dp->d_name;
            current = (file_list*) malloc(sizeof(file_list));
            current->filename = (char*) malloc(strlen(directory)+(strlen(file_name)+2)*sizeof(char));
            strcpy(current->filename,directory);
            strcat(current->filename,"/");
            strcat(current->filename,file_name);
            current->next = head;
            head = current;
            //printf("file_name: \"%s\"\n",current->filename);
            count++;
        }
    }

    closedir(dir);
    return head;
}

/*
*	Generate random image weight array of floats [0,1]
*	of length n, using a random seed
*/
float *rand_image_weights(int n, int seed, float &sum)
{
	srand(seed);

	sum=0;

	//Initialize weight array
	float *weights = (float*) malloc(sizeof(float)*n); 

	for(int i=0;i<n;i++)
	{
		weights[i]= rand() % 10;
		//printf("Weight of Image %d is %.2f\n",i, weights[i]);
		sum+=weights[i];
	}

	return weights;
}

int main (int argc, char* argv[])
{

	//TODO: getopt parsing

	int filecount;

	printf("Reading directory: %s\n",argv[1]);
	file_list *files = read_files(argv[1], filecount);
	file_list *ptr = files;

	printf("File Count: %d\n", filecount);

	if(argc==3) //If thread count is specified
	{
		printf("Setting Thread Count to %d\n",atoi(argv[2]));
		omp_set_num_threads(atoi(argv[2]));
	}

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

	
}





