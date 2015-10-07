#!/bin/python

from skimage import io
from scidbpy import connect

import sys
import os
import numpy

import timer

#Helper function to get 
def absoluteFilePaths(directory):
   for dirpath,_,filenames in os.walk(directory):
       for f in filenames:
           yield os.path.abspath(os.path.join(dirpath, f))

#Default Values
OUTPUT_FILE = 'output.jpg'
HANDLE_OVERFLOW = True
TIMING_INFO = ''

#Connect to localhost SciDB instance thorugh shim layer
with timer.Timer() as t:
	sdb = connect('http://localhost:8080/')
TIMING_INFO+='Initiating SciDB connection :%.03f' % t.interval+' sec\n'

#Use PIL 
io.use_plugin('pil')

def read_image_into_scidb(img_file):
	global TIMING_INFO
	print 'Reading: '+img_file

	with timer.Timer() as t:
		image_array = io.imread(img_file)
	TIMING_INFO+='Reading image file: '+img_file+' :%.03f' % t.interval+' sec\n'

	with timer.Timer() as t:
		#Store image in SciDB
		if HANDLE_OVERFLOW: #Extend to 32-bit int for summation
			image_array_sdb = sdb.from_array(image_array.astype(numpy.uint32))
		else: #Don't care, let it overflow.
			image_array_sdb = sdb.from_array(image_array)

	TIMING_INFO+='Storing image file in DB: '+img_file+' :%.03f' % t.interval+' sec\n'
		

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
	with timer.Timer() as t:
		sum_image = generate_scidb_result_array(next(image_files))
	TIMING_INFO+='Reading first file and generating empty array :%.03f' % t.interval+' sec\n'


	count = 0
	#for each file in the source directory
	for img_file in absoluteFilePaths(sys.argv[1]):
		
		image_array_sdb = read_image_into_scidb(img_file)
		
		#TODO: Image dimension validation here
		with timer.Timer() as t:
			sum_image = sum_image + image_array_sdb
		TIMING_INFO+='Adding image file to Sum: '+img_file+' :%.03f' % t.interval+' sec\n'
		with timer.Timer() as t:
			image_array_sdb.reap() #Reap this image
		TIMING_INFO+='Reaping Image: '+img_file+' :%.03f' % t.interval+' sec\n'

		count+=1

	#Divide by total # of images to get average
	with timer.Timer() as t:
		sum_image = sum_image / count
	TIMING_INFO+='Scalar division on result array :%.03f' % t.interval+' sec\n'

	with timer.Timer() as t:
		result_array = sum_image.toarray()
	TIMING_INFO+='Fetching result array from SciDB :%.03f' % t.interval+' sec\n'

	with timer.Timer() as t:
		io.imsave(OUTPUT_FILE, result_array.astype(numpy.uint8))
	TIMING_INFO+='Writing final output to disk :%.03f' % t.interval+' sec\n'

	print 'Output written to: '+OUTPUT_FILE

	print '\nTiming Info:\n============'
	print TIMING_INFO
