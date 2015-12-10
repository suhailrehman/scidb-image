#!/bin/bash



#DIRECTORY="/home/scidb/input/64x64/10"

#if [ -z "$1" ]
#  then
#	DIRECTORY="/home/scidb/input/64x64/10"
#else
#	DIRECTORY="$1"
#fi

#python image_load.py $DIRECTORY

time iquery -vnq "SELECT * INTO temp_average FROM image_volume JOIN weights ON image_volume.i3 = weights.i0;"
time iquery -vnaq "store(aggregate(apply(temp_average,weighted_image,f0*f0_2),sum(weighted_image),i0,i1,i2),average_image);"

python image_writeout.py average_image
