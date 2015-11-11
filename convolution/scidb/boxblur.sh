#!/bin/bash

#Use Windowed Aggregate
time iquery -naq "window(slice(image_volume,i3,0),0,2,0,2,0,0,sum(f0));"