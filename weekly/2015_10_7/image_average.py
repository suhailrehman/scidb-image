#!/bin/python

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

#Default Values
OUTPUT_FILE = 'output.jpg'
HANDLE_OVERFLOW = True

#Connect to localhost SciDB instance thorugh shim layer
sdb = connect('http://localhost:8080/')

#Use PIL 
io.use_plugin('pil')

def read_image_into_scidb(img_file):
	print 'Reading: '+img_file
	image_array = io.imread(img_file)

	#Store image in SciDB
	if HANDLE_OVERFLOW: #Extend to 32-bit int for summation
		image_array_sdb = sdb.from_array(image_array.astype(numpy.uint32))
	else: #Don't care, let it overflow.
		image_array_sdb = sdb.from_array(image_array)

	return image_array_sdb

def generate_scidb_result_array(img_file):
	image_array = io.imread(img_file)
	image_array_sdb = sdb.from_array(image_array)

	print 'Dimensions: ' + image_array_sdb.schema

	print 'Creating Result SciDB array based on these dimensions'
	if 'sum_image' in sdb.list_arrays():
		sdb.query("remove(sum_image)")
	sdb_create_query = "store(build(%s,0),sum_image)" % image_array_sdb.schema

	if HANDLE_OVERFLOW:
		sdb_create_query.replace('uint8','uint32')

	sdb.query(sdb_create_query)
	return sdb.wrap_array('sum_image')


if __name__ == "__main__":
	#Parse input arguments
	if len(sys.argv)<2:
		print "Error: Enter input directory..."
	if len(sys.argv)==3:
		OUTPUT_FILE=sys.argv[2]

	#Enumerate input directory:
	image_files = absoluteFilePaths(sys.argv[1])

	#Get the schema for the first image and create the result image in SciDB:
	print 'Getting dimensions of first image in directory...'
	sum_image = read_image_into_scidb(next(image_files))

	count = 0
	#for each file in the source directory
	for img_file in image_files:
		
		image_array_sdb = read_image_into_scidb(img_file)
		
		#TODO: Image dimension validation here
		sum_image = sum_image + image_array_sdb
		image_array_sdb.reap() #Reap this image
		count+=1

	#Divide by total # of images to get average
	sum_image = sum_image / count

	result_array = sum_image.toarray()

	io.imsave(OUTPUT_FILE, result_array.astype(numpy.uint8))
	print 'Output written to: '+OUTPUT_FILE
