#!/bin/bash

PATCH_X_START=100
PATCH_X_END=199
PATCH_Y_START=200
PATCH_Y_END=299


time iquery -naq "store(aggregate(apply(cross_join(subarray(image_volume,$PATCH_X_START,$PATCH_Y_START,0,0,$PATCH_X_END,$PATCH_Y_END,2,6) as patch_volume, weights,patch_volume.i3,weights.i0),weighted_image,f0*weight),sum(weighted_image),i0,i1,i2),average_patch);"
