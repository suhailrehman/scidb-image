from skimage import io
#from scidbpy import connect

#scb = connect('http://localhost:8080/')

image_array = io.imread('test.jpg')
io.imsave('test_output.jpg',image_array)
