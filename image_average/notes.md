Weighted Image Patch Operation:

patch_width = 100;
patch_height = 100;

Given Image Volume(width,height,channel,img_num)
For every img in voulme:
	
	width_min = RAND(0,image_width-patch_width);
	height_min = RAND(0,image_height-patch_height);
	
	img = slice(volume, img_num = img)
	patch = subarray(img,width_min,width_min+patch_width,height_min,height_min+patch_height)

	//SciDB:
	//store(subarray(slice(image_volume,i3,$IMG_NUM),0,100,0,100,0,2),patch);



	append(patch, patch_volume)
	

	SELECT * INTO patch_volume_temp FROM patch_volume JOIN weights ON patch_volume.i3 = weights.i0;
	store(aggregate(apply(patch_volume_temp,weighted_patch,f0*weight),sum(weighted_patch),i0,i1,i2),average_patch);


