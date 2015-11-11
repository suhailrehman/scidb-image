#include <stdio.h>
#include "CImg.h"
#include <time.h>
#include <stdlib.h>

#include <omp.h>

#define KERNEL_HEIGHT 3
#define KERNEL_WIDTH 3
 
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

	float kernel[KERNEL_HEIGHT][KERNEL_WIDTH] = {  
	   {1,1,1} ,   /*  initializers for row indexed by 0 */
	   {1,1,1} ,   /*  initializers for row indexed by 1 */
	   {1,1,1}   /*  initializers for row indexed by 2 */
	};
	
	//Load first image to get dimensions - 
	//TODO: replace with canvas properties for the future
	CImg<unsigned char> src(argv[1]);
	int width = src.width();
	int height = src.height();
	int channels = src.spectrum();
	
	//Create initial black image
	CImg<double> output (width,height,1,3,0);

	int count = 0;
	double start_time, weighted_sum_time=0, scalar_divide_time=0;

	start_time = omp_get_wtime();

	#pragma omp parallel for 
	cimg_forXYC(output,x,y,c) 
	{
   		//avg(x,y,c) = avg(x,y,c) + (next(x,y,c) * weights[count]);
	 	float sum= 0.0;
 		
 		for(int i = 0; i < KERNEL_HEIGHT; i++)
 		{
 			for(int j = 0; j < KERNEL_WIDTH; j++)
 			{
 				if((x-(KERNEL_WIDTH/2)+j) > 0 && (x-(KERNEL_WIDTH/2)+j) < width && (y-(KERNEL_HEIGHT/2)+i) > 0 && (y-(KERNEL_HEIGHT/2)+i) < height)
				{
					
					int xcoord = x-(KERNEL_WIDTH/2)+j;
					int ycoord = y-(KERNEL_HEIGHT/2)+i;
					sum += src(xcoord,ycoord,c)*kernel[i][j];
				}
 			}
 		}

 		output(x,y,c)=sum;

	}

	weighted_sum_time += (omp_get_wtime() - start_time);

	//TODO: Custom Output image save
	output.save("output.jpg");

	
}





