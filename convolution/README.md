convolution:

```
for  i in image rows:
	for  j in image cols:
		for  c in image channels:
			for x in kernel rows:
				for y in kernel cols:
					 if(i-(kernel.width/2)+x>0 && i-(kernel.width/2)+x <image.width && i-(kernel.height/2)+y>0 && i-(kernel.height/2)+y<image.height):
						 value = image((i-(kernel.width/2))+x,(j-(kernel.height/2)+y, channel)
						 value = value * kernel(x,y)
						 image((i-(kernel.width/2))+x,(j-(kernel.height/2)+y, channel) += value
```