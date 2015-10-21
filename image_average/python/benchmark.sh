#!/bin/bash

(./image_average.sh ~/Downloads/thumbs10/  ) &>> ../../weekly/2015_10_21/scidb_thumbs10.log
(./image_average.sh ~/Downloads/thumbs100/  ) &>> ../../weekly/2015_10_21/scidb_thumbs100.log
(./image_average.sh ~/Downloads/thumbs1000/  ) &>> ../../weekly/2015_10_21/scidb_thumbs1000.log
(./image_average.sh ~/Downloads/BSDS/10-horizontal )  &>> ../../weekly/2015_10_21/scidb_BSDS_10_horizontal.log
(./image_average.sh ~/Downloads/BSDS/100-horizontal )  &>> ../../weekly/2015_10_21/scidb_BSDS_100_horizontal.log
(./image_average.sh ~/Downloads/fhdsample2 )  &>> ../../weekly/2015_10_21/scidb_fhdsample2.log
(./image_average.sh ~/Downloads/fhdsample10 )  &>> ../../weekly/2015_10_21/scidb_fhdsample10.log
