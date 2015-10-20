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
image_array_sdb = sdb.from_array(average, persistent=True)


#Print out name of image
print "SciDB Array Name: "+image_array_sdb.name 

# remove A if it already exists
if "average_img" in sdb.list_arrays():
    sdb.query("remove(average_img)")

# create an array named 'A' on the server
sdb.query("store(aggregate(%s,avg(f0),i0,i1,i2),average_img)"%image_array_sdb.name)

# create a Python object pointing to this array
average_img = sdb.wrap_array("average_img")

#Write out image to file
result_array = average_img.toarray()
io.imsave(OUTPUT_FILE, result_array.astype(numpy.uint8))			