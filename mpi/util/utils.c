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
