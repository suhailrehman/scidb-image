from skimage import io
from scidbpy import connect

import sys
import os
import numpy
import random

#argv[1] = image_volume name in SciDB
patch_width = 100;
patch_height = 100;

sdb = connect('http://localhost:8080/')

if sys.argv[1] in sdb.list_arrays():
     image_volume = sdb.wrap_array(sys.argv[1])

image_height = image_volume.shape[1]
image_width= image_volume.shape[2]

num_images = image_volume.shape[-1]

if "patch_volume" in sdb.list_arrays():
    sdb.query("remove(patch_volume)")

sdb.query("CREATE ARRAY patch_volume<f0:double>[i0=0:%s,1000,0,i1=0:%s,1000,0,i2=0:2,1000,0,i3=0:%s,1000,0]"%(patch_height-1,patch_width-1,num_images));
patch_volume = sdb.wrap_array("patch_volume")

for image_num in range(num_images):
	
	#Get Rand Height Start
	patch_x_start = random.randint(0, image_width)
	patch_y_start = random.randint(0, image_height)
	#Get Rand Width Start

	patch_x_end = patch_x_start + patch_width
	patch_y_end = patch_y_start + patch_height

	image_patch = image_volume[patch_x_start:patch_x_end,patch_y_start:patch_y_end,:,image_num]

	sdb.query("insert(redimension(apply(%s, i3, %s), patch_volume), patch_volume)" %(image_patch.name ,image_num));

	print patch_volume
	print image_patch




	#for num in image_volume.
#image_volume.isel(i3=num).toarray()


#Given Image Volume(width,height,channel,img_num)

#CREATE ARRAY patch_volume<f0:int64>[i0=0:99,1000,0,i1=0:99,1000,0,i2=0:2,1000,0,i3=0:*,1000,0]
