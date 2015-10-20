from skimage import io
from scidbpy import connect

#Connect to localhost SciDB instance thorugh shim layer
sdb = connect('http://localhost:8080/')

#Read local image into numpy_array
image_array = io.imread('test.jpg')

#Write image to SciDB
image_array_sdb = sdb.from_array(image_array, persistent=True)

#Print out name of image
print "SciDB: image_array_sdb.name"+image_array_sdb.name 


#io.imsave('test_output.jpg',image_array)
