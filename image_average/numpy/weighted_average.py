from skimage import io
import random

import sys
import os
import numpy

OUTPUT_FILE = 'output.jpg'

#Helper function to get file paths of each image
def absoluteFilePaths(directory):
   for dirpath,_,filenames in os.walk(directory):
       for f in filenames:
           yield os.path.abspath(os.path.join(dirpath, f))


if len(sys.argv)<2:
		print "Error: Enter input directory..."
if len(sys.argv)==3:
	OUTPUT_FILE=sys.argv[2]

#Enumerate input directory:
image_files = absoluteFilePaths(sys.argv[1])
num_files = len(list(image_files))
print str(num_files) + " files in directory"

average = None
weights = random.sample(xrange(1, 10), num_files)

#Load images into Target Array
#for each file in the source directory
count = 0
for img_file in absoluteFilePaths(sys.argv[1]):
  if count==0:
    image = io.imread(img_file)
    average = numpy.empty(image.shape)
    print average.dtype
  average = average + (io.imread(img_file) * weights[count])
  count=count+1
    
average = average / sum(weights)


io.imsave(OUTPUT_FILE, average)			