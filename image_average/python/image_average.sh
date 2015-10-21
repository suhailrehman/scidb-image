#!/bin/bash


PATH=$PATH:"/home/scidb/scidb/scidbtrunk/stage/install/bin"

DIRECTORY="/home/scidb/Downloads/thumbs10/"


WEIGHTTOTAL=`python image_average.py $DIRECTORY | tail -n 1`

echo $WEIGHTTOTAL

iquery -naq "remove(temp_average)"
iquery -naq "remove(average_image2)"

time iquery -nq "SELECT * INTO temp_average FROM average_img JOIN weights ON average_img.i3 = weights.i0;"
time iquery -naq "store(aggregate(apply(temp_average,weighted_image,f0*weight),sum(weighted_image),i0,i1,i2),average_image);"
#iquery -aq "show(average_image)"
time iquery -naq "store(project(apply(average_image,weighted_image_avg, weighted_image_sum / $WEIGHTTOTAL ),weighted_image_avg),average_image2)"
#iquery -aq "show(average_image2)"

python image_writeout.py average_image2