#include <stdio.h>
#include "CImg.h"

using namespace cimg_library;

//C Program outline

int main (int argc, char* argv[])
{

	//TODO: Quit if image not specified

	//Load Image from input parameter
	CImg<unsigned char> src(argv[1]);
	int width = src.width();
	int height = src.height();
	
	//Create initial black image
	CImg<unsigned char> avg (width,height,1,3,0);

	//TODO: omp pragma for
	cimg_forXYC(avg,x,y,c) {  // Do 3 nested loops
   		avg(x,y,c) = avg(x,y,c) + src(x,y,c); 
   		avg(x,y,c) = avg(x,y,c) / 2;
	}


	avg.save("output.jpg");


}
