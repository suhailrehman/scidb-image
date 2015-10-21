from skimage import io
from scidbpy import connect

import sys
import os
import numpy

OUTPUT_FILE = 'output.jpg'

#Helper function to get file paths of each image
def absoluteFilePaths(directory):
   for dirpath,_,filenames in os.walk(directory):
       for f in filenames:
           yield os.path.abspath(os.path.join(dirpath, f))

#Connect to localhost SciDB instance thorugh shim layer
sdb = connect('http://localhost:8080/')

if len(sys.argv)<2:
		print "Error: Enter input directory..."
if len(sys.argv)==3:
	OUTPUT_FILE=sys.argv[2]

#Enumerate input directory:
image_files = absoluteFilePaths(sys.argv[1])
num_files = len(list(image_files))
print str(num_files) + " files in directory"

#create random weights
if "average_img" in sdb.list_arrays():
     sdb.query("remove(average_img)")

average = None

#Load images into Target Array
#for each file in the source directory
count = 0
for img_file in absoluteFilePaths(sys.argv[1]):
	if count==0:
		image = io.imread(img_file)
		average = numpy.empty(tuple(list(image.shape) + [num_files]))
	
	average[:,:,:,count] = io.imread(img_file)
	count+=1

#Write array of images to SciDB
image_array_sdb = sdb.from_array(average, persistent=True, name="average_img")

#create random weights
if "weights" in sdb.list_arrays():
    sdb.query("remove(weights)")

sdb.query("CREATE ARRAY weights <weight:double>[i0=0:%s,10,0]"%count)
sdb.query("store(build(weights,double(random()%10)),weights)");
weights = sdb.wrap_array("weights")

#Print out name of image
print "SciDB Array Name: "+image_array_sdb.name 

print weights.sum()[0]