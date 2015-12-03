Slice along fixed patch coordinates:
====================================

#Assuming Image Volume is in image_volume:
$PATCH_X_START = 100
$PATCH_X_END = 199
$PATCH_Y_START = 200
$PATCH_Y_END = 299

store(subarray(image_volume,$PATCH_X_START,$PATCH_Y_START,0,0,$PATCH_X_END,$PATCH_Y_END,2,1),patch_volume);


//TODO: Better way around this...
store(cast(weights, <weight:double> [i0=0:6,1000,0]),weights2);
remove(weights);
rename(weights2,weights);


#cross_join weights
store(cross_join(patch_volume, weights,patch_volume.i3,weights.i0),patch_volume_joined);

#aggregation
store(aggregate(apply(patch_volume_joined,weighted_image,f0*weight),sum(weighted_image),i0,i1,i2),average_patch);"


Final Combined Query:
aggregate(apply(cross_join(subarray(image_volume,$PATCH_X_START,$PATCH_Y_START,0,0,$PATCH_X_END,$PATCH_Y_END,2,$NUM_FILES) as patch_volume, weights,patch_volume.i3,weights.i0),weighted_image,f0*weight),sum(weighted_image),i0,i1,i2);

Example:
store(aggregate(apply(cross_join(subarray(image_volume,100,200,0,0,199,299,2,6) as patch_volume, weights,patch_volume.i3,weights.i0),weighted_image,f0*weight),sum(weighted_image),i0,i1,i2),average_patch);



Image Patch along variable patch coordinates
============================================

patch_width = 100;
patch_height = 100;

Given Image Volume(width,height,channel,img_num)
For every img in voulme:
	
	width_min = RAND(0,image_width-patch_width);
	height_min = RAND(0,image_height-patch_height);
	
	img = slice(volume, img_num = img)
	patch = subarray(img,width_min,width_min+patch_width,height_min,height_min+patch_height)

	insert(redimension(apply(patch, i3, 1), patch_volume), patch_volume)



	SELECT * INTO patch_volume_temp FROM patch_volume JOIN weights ON patch_volume.i3 = weights.i0;
	store(aggregate(apply(patch_volume_temp,weighted_patch,f0*weight),sum(weighted_patch),i0,i1,i2),average_patch);



Once patch is constructed:

store(aggregate(apply(cross_join(patch_volume, weights,patch_volume.i3,weights.i0),weighted_image,f0*weight),sum(weighted_image),i0,i1,i2),average_patch);"



Convolution
===========

Built-In SciDB Window Operation on Image Volume:
window(image_volume,0,2,0,2,0,0,0,0,sum(f0));  

Single Image Window Operation from a Volume:
window(slice(image_volume,i3,0),0,2,0,2,0,0,sum(f0));
