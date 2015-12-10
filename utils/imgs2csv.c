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


int main (int argc, char* argv[])
{

	//TODO: getopt parsing

	int filecount;

	fprintf (stderr,"Reading directory: %s\n",argv[1]);
	file_list *files = read_files(argv[1], filecount);
	file_list *ptr = files;

	fprintf (stderr,"File Count: %d\n", filecount);

	//Load first image to get dimensions - 
	//TODO: replace with canvas properties for the future
	CImg<unsigned char> src(files->filename);
	int width = src.width();
	int height = src.height();
	int channels = src.spectrum();
	
	int count = 0;
	//TODO: Loop over all images in directory
	while(ptr!=NULL)
	{
		//Load next image
		CImg<unsigned char> next(ptr->filename);

		fprintf (stderr,"Reading file %d: %s\r",count+1, ptr->filename);


		if(next.width() != width || next.height() != height || next.spectrum() != channels || next.depth() !=src.depth())
		{
			fprintf (stderr,"Bad File: %s\n", ptr->filename);
			ptr=ptr->next;
			continue;
		}

		#pragma omp parallel for 
		cimg_forXYC(next,x,y,c) 
		{
	   		printf("%d,%d,%d,%d,%d\n",x,y,c,count,next(x,y,c)); 

		}

 		ptr=ptr->next;
		count++;
	}

	
	fprintf (stderr,"\nComplete!\n");
	fprintf (stderr,"image_volume<f0:double>[i0=0:%d,5000,0,i1=0:%d,5000,0,i2=0:%d,1000,0,i3=0:%d,1,0]\n",width-1,height-1,channels-1,count-1);

	
}





