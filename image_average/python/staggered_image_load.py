from skimage import io
from scidbpy import connect

import sys
import os
import numpy
import random


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

if "image_volume" in sdb.list_arrays():
    sdb.query("remove(image_volume)")

count = 0
for img_file in absoluteFilePaths(sys.argv[1]):
	image = io.imread(img_file)
	if count==0:
		img_shape = image.shape
		sdb.query("CREATE ARRAY image_volume<f0:double>[i0=0:%s,5000,0,i1=0:%s,5000,0,i2=0:%s,1000,0,i3=0:%s,1,0]"%(img_shape[0],img_shape[1],img_shape[2],num_files));
		image_volume = sdb.wrap_array("image_volume")
	if(img_shape==image.shape):
		if "single_image" in sdb.list_arrays():
    			sdb.query("remove(single_image)")
		sdb.from_array(image.astype('float64'),persistent=False, name="single_image")
		sdb.query("insert(redimension(apply(single_image, i3, %s), image_volume), image_volume)" %count);
	else:
		print "!Bad image: ",img_file
	count+=1
