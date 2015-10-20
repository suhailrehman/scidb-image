from skimage import io
from scidbpy import connect

import sys

#Connect to localhost SciDB instance thorugh shim layer
sdb = connect('http://localhost:8080/')

#assume argument 1 is the name of the Array in SciDB
array_name = sys.argv[1]
#print sdb.list_arrays()

if array_name in sdb.list_arrays():
	sdb_array  = sdb.wrap_array(array_name)
	image_array = sdb_array.toarray()
	io.imsave(sys.argv[2],image_array)
