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


#endif /* UTIL_UTILS_H_ */
