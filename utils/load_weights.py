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
sdb.query("CREATE ARRAY weights <weight:double>[i0=0:%s,10,0]"%str(num_files-1))
sdb.query("store(build(weights,random()%10),weights)");
weights = sdb.wrap_array("weights")
weight_sum = weights.sum()[0]
weights = weights / weight_sum #Normalize array weights.
weights.attribute_rename("x","weight")
print weights.query
weights.eval()
print "Weighted Array: ",weights.name
weights = sdb.wrap_array(weights.name)

sdb.query("remove(weights)")
sdb.query("store(cast(%s, <weight:double> [i0=0:%s,1000,0]),weights)"%(weights.name,str(num_files-1)));





