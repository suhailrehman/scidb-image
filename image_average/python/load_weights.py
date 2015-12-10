from skimage import io
from scidbpy import connect

import sys
import os
import numpy
import random

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
if "image_volume" in sdb.list_arrays():
     sdb.query("remove(image_volume)")

volume = None

#Load images into Target Array
#for each file in the source directory
count = 0
for img_file in absoluteFilePaths(sys.argv[1]):
	image = io.imread(img_file)
	if count==0:
		img_shape = image.shape
		volume = numpy.empty(tuple(list(image.shape) + [num_files]))
	if(img_shape==image.shape):
		volume[:,:,:,count] = io.imread(img_file)
	else:
		print "!Bad image",img_file
	count+=1

#Write array of images to SciDB
image_array_sdb = sdb.from_array(volume, persistent=True, name="image_volume")

#create random weights
if "weights" in sdb.list_arrays():
    sdb.query("remove(weights)")

'''
sdb.query("CREATE ARRAY weights <weight:double>[i0=0:%s,10,0]"%count)
sdb.query("store(build(weights,random()%10),weights)");
weights = sdb.wrap_array("weights")
weight_sum = weights.sum()[0]
weights = weights / weight_sum #Normalize array weights.
print weights.sum()[0]
weights = sdb.wrap_array("weights") 
'''
# Generate and normalize weights using numpy:
weights = [random.randint(1, 10) for x in xrange(num_files)]
weight_sum = sum(weights)
weights = [float(weight) / weight_sum for weight in weights]
weight_array = sdb.from_array(weights, persistent=True, name="weights")
weight_array.attribute_rename("f0","weight")
print weight_array.query
weight_array.eval()



#Print out name of image
print "SciDB Array Name: "+image_array_sdb.name 
