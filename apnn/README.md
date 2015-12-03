#All pairs nearest neighbors in SciDB

Compute nearest neighbors for all pairs of images in an image volume in SciDB, using the Sum of Squared Distances as the distance metric.

## Flow:
Join Image Volume by itself to produce all image pairs
Compute attribute 3 = (attribute 1 - attribute 2)^2 (SSD)
Aggregate attribute 3 along imagenumber axis and store as new array
redimension new array as imagenum x imagenum
apply knn UDO on 2D array of image distances.

## Code:
'''
SELECT * INTO image_pairs FROM image_volume as iv1 JOIN image_volume as iv2 ON iv1.i0 = iv2.i0 AND iv1.i1 = iv2.i1 AND iv1.i2 = iv2.i2;
set lang afl;
store(apply(image_pairs, f0_square, pow(f0-f0_2,2)),image_pairs_square_diff); #Compute squared difference
CREATE ARRAY image_pairs_ssd <f0_square_sum: double> [image_1=0:$NUM_IMAGES,1000,0, image_2=0:$NUM_IMAGES,$NUM_IMAGES,0];
store(redimension(aggregate(square_diff,sum(square_diff),i3),image_pairs_ssd,image_pairs_ssd) #compute SSD
knn(image_pairs_ssd, $MAX_K)
'''





