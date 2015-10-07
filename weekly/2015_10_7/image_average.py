from skimage import io
from scidbpy import connect

import sys
import os
import numpy

#Helper function to get 
def absoluteFilePaths(directory):
   for dirpath,_,filenames in os.walk(directory):
       for f in filenames:
           yield os.path.abspath(os.path.join(dirpath, f))

#TODO populate values using the first image in the series.
IMAGE_X_DIM = 512
IMAGE_Y_DIM = 512
OUTPUT_FILE = 'output.jpg'

#Connect to localhost SciDB instance thorugh shim layer
sdb = connect('http://localhost:8080/')

#Use PIL 
io.use_plugin('pil')

#TODO: Default: /home/scidb/Downloads/sample
if not sys.argv[1]:
	print "Error: Enter input directory..."

#Create the result image i SciDB:
#sdb.query(store(build(<f0:uint64>[i0=0:99,10,0,i1=0:99,10,0,i2=0:2,10,0],0,sum_array));
if 'sum_image' in sdb.list_arrays():
    sdb.query("remove(sum_image)")
sdb.query("store(build(<f0:uint32>[i0=0:511,1000,0,i1=0:511,1000,0,i2=0:2,1000,0],0),sum_image)")
sum_image = sdb.wrap_array('sum_image')
print 'sum_image: '+ sum_image.schema

count = 0
#for each file in input argument
for img_file in absoluteFilePaths(sys.argv[1]):
	count+=1
	image_array = io.imread(img_file)
	image_array_sdb = sdb.from_array(image_array.astype(numpy.uint32))
	print img_file+': ' + image_array_sdb.schema
	sum_image = sum_image + image_array_sdb
	image_array_sdb.reap()

sum_image = sum_image / count

result_array = sum_image.toarray()

io.imsave(OUTPUT_FILE, result_array.astype(numpy.uint8))


