from skimage import io
from scidbpy import connect

import sys
import os
import numpy

OUTPUT_FILE = 'output.jpg'


'''
SciDB Query:

SELECT * INTO temp_average FROM average JOIN weights ON average.i3 = weights.i0;
aggregate(apply(temp_average,weighted_image,f0*weight),sum(weighted_image),i0,i1,i2);

'''


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

#create random weights
if "weights" in sdb.list_arrays():
    sdb.query("remove(weights)")

sdb.query("CREATE ARRAY weights <weight:double>[i0=0:%s,10,0]"%count)
sdb.query("store(build(weights,double(random()%10)/double(10)),weights)");
weights = sdb.wrap_array("weights")

#Print out name of image
print "SciDB Array Name: "+image_array_sdb.name 

if "average_img" in sdb.list_arrays():
    sdb.query("remove(average_img)")

sdb.query("SELECT * INTO average_img FROM %s JOIN weights ON %s.i3 = weights.i0"%(image_array_sdb.name,image_array_sdb.name));#
print image_array_sdb.dim_names
#sdb.cross_join(image_array_sdb, weights)

#sdb.merge(image_array_sdb, weights, left_on='f0', right_on='weight')
#sdb.query("store(aggregate(%s,avg(f0),i0,i1,i2),average_img)"%image_array_sdb.name)
print image_array_sdb.att_names
sdb.query("store(aggregate(apply(%s,weighted_image,f0*weight),sum(weighted_image),i0,i1,i2),average_img)"%image_array_sdb.name)


# create a Python object pointing to this array
average_img = sdb.wrap_array("average_img")

#Write out image to file
result_array = average_img.toarray()
io.imsave(OUTPUT_FILE, result_array.astype(numpy.uint8))			