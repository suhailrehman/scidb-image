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
weights = [random.randint(1, 10) for x in xrange(num_files)]
weight_sum = sum(weights)

weights = [float(weight) / weight_sum for weight in weights]

print "Weights: "+str(weights);

#Load images into Target Array
#for each file in the source directory
count = 0
for img_file in absoluteFilePaths(sys.argv[1]):
  if count==0:
    image = io.imread(img_file)
    print image.shape
    average = numpy.empty(image.shape)
  image = (io.imread(img_file).astype(numpy.float64))
  image = image * weights[count]
  if(image.shape != average.shape):
    print "!Skipping file: ", img_file
  else:
    average = average + image
  count=count+1
    

io.imsave(OUTPUT_FILE, average.astype(numpy.uint8))			