/*
 * utils.h
 *
 *  Created on: Dec 7, 2015
 *      Author: suhailr
 */

#ifndef UTIL_UTILS_H_
#define UTIL_UTILS_H_



std::vector<std::string> read_files(char *directory);
void *rand_image_weights(int n, int seed, float &sum, float *weights);

//MPI Blocking Primitives
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) \
                     (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)
#define BLOCK_OWNER(j,p,n) (((p)*((j)+1)-1)/(n))


#endif /* UTIL_UTILS_H_ */
