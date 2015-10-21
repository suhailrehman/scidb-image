from skimage import io
from scidbpy import connect

import sys
import os
import numpy

OUTPUT_FILE = 'output.jpg'

#Connect to localhost SciDB instance thorugh shim layer
sdb = connect('http://localhost:8080/')

# create a Python object pointing to this array
average_img = sdb.wrap_array(sys.argv[1])

#Write out image to file
result_array = average_img.toarray()
io.imsave(OUTPUT_FILE, result_array.astype(numpy.uint8))			